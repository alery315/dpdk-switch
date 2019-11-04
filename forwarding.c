#include "main.h"

int
app_l2_learning(const struct ether_addr* srcaddr, uint8_t port) {
    if (app.l2_hash == NULL) {
        // hash表没有初始化
        RTE_LOG(
            ERR, HASH,
            "%s: ERROR hash table is not initialized.\n",
            __func__
        );
        return -1;
    }
    int index = rte_hash_lookup(app.l2_hash, srcaddr);
    if (index == -EINVAL) {
        // 参数错误
        RTE_LOG(
            ERR, HASH,
            "%s: ERROR the parameters are invalid when lookup hash table\n",
            __func__
        );
    } else if (index == -ENOENT) {
        // 没有找到
        // 向l2.hash新加一个src_addr key,返回一个正值,可以作为数组偏移,对key唯一
        // 返回一个hash值与src_addr一一对应
        int new_ind = rte_hash_add_key(app.l2_hash, srcaddr);
        app.fwd_table[new_ind].port_id = port;
        // the time when item is added
        app.fwd_table[new_ind].timestamp = rte_get_tsc_cycles();
        /* gettimeofday(&app.fwd_table[new_ind].timestamp, NULL); */
        RTE_LOG(
            INFO, HASH,
            "%s: new item in forwarding table:"
            " %02" PRIx8 " %02" PRIx8 " %02" PRIx8
            " %02" PRIx8 " %02" PRIx8 " %02" PRIx8
            " --> %d\n",
            __func__,
            srcaddr->addr_bytes[0], srcaddr->addr_bytes[1],
            srcaddr->addr_bytes[2], srcaddr->addr_bytes[3],
            srcaddr->addr_bytes[4], srcaddr->addr_bytes[5],
            app.ports[port]
        );
    } else if (unlikely(index < 0 || index >= FORWARD_ENTRY)) {
        // 发生了错误吧,不在范围内
        RTE_LOG(
            ERR, HASH,
            "%s: ERROR invalid table entry found in hash table: %d\n",
            __func__, index
        );
        return -1;
    } else {
        // 找到了,在范围内
        // 先取出old_port,临时变量
        int old_port = app.fwd_table[index].port_id;
        // 直接更新port和插入时间戳
        app.fwd_table[index].port_id = port;
        app.fwd_table[index].timestamp = rte_get_tsc_cycles();
        // gettimeofday(&app.fwd_table[index].timestamp, NULL);
        // 前后port不一样,更新,src_addr还是相同,但是他的port变了
        if (old_port != port) {
            RTE_LOG(
                INFO, HASH,
                "%s: Update item in forwarding table:"
                " %02" PRIx8 " %02" PRIx8 " %02" PRIx8
                " %02" PRIx8 " %02" PRIx8 " %02" PRIx8
                "--> %d (previous: %d)\n",
                __func__,
                srcaddr->addr_bytes[0], srcaddr->addr_bytes[1],
                srcaddr->addr_bytes[2], srcaddr->addr_bytes[3],
                srcaddr->addr_bytes[4], srcaddr->addr_bytes[5],
                app.ports[port], app.ports[old_port]
            );
        }
    }
    return 0;
}

int
app_l2_lookup(const struct ether_addr* addr) {
    int index = rte_hash_lookup(app.l2_hash, addr);
    if (index >= 0 && index < FORWARD_ENTRY) {
        // 范围内的情况

        // 获得当前时钟周期
        uint64_t now_time = rte_get_tsc_cycles();
        // 算出距离上次访问的时间戳差值
        uint64_t interval = now_time - app.fwd_table[index].timestamp;
        // 在设定的有效实际内(好像是个很大的数字)
        if (interval <= app.fwd_item_valid_time) {
            return app.fwd_table[index].port_id;
        } else {
            // 否则打印过期错误,并删除从hash中删除这个key
            RTE_LOG(
                INFO, HASH,
                "%s: Fowllowing item is outdated, delete it from forwarding table:"
                " %02" PRIx8 " %02" PRIx8 " %02" PRIx8
                " %02" PRIx8 " %02" PRIx8 " %02" PRIx8
                "--> %d\n",
                __func__,
                addr->addr_bytes[0], addr->addr_bytes[1],
                addr->addr_bytes[2], addr->addr_bytes[3],
                addr->addr_bytes[4], addr->addr_bytes[5],
                app.ports[app.fwd_table[index].port_id]
            );
            // 删除对应的key,返回-1
            rte_hash_del_key(app.l2_hash, addr);
            return -1;
        }
        /*struct timeval now_time, intv_time;
        gettimeofday(&now_time, NULL);
        timersub(&now_time, &app.fwd_table[index].timestamp, &intv_time);
        long intv_time_us = intv_time.tv_sec * 1000 * 1000 + intv_time.tv_usec;
        if (intv_time_us / 1000 < VALID_TIME) {
            return app.fwd_table[index].port_id;
        } else {
            rte_hash_del_key(app.l2_hash, addr);
            return -1;
        }*/
    }
    return -1;
}

void
app_main_loop_forwarding(void) {
    // forward的mbuf结构体
    struct app_mbuf_array *worker_mbuf;
    // 以太网头部,目的,源,帧类型
    struct ether_hdr *eth;
    // 通用rte_mbuf,包含数据包的mbuf,它的指针
    struct rte_mbuf* new_pkt;
    uint32_t i, j;
    int dst_port;

    RTE_LOG(INFO, SWITCH, "Core %u is doing forwarding\n",
        rte_lcore_id());

    // cpu_freq[lcore] = cpu自启动后的运行时钟周期
    app.cpu_freq[rte_lcore_id()] = rte_get_tsc_hz();
    // 转发item有效时间 时钟周期 / 1000 * 整型最大值
    app.fwd_item_valid_time = app.cpu_freq[rte_lcore_id()] / 1000 * VALID_TIME;

    // 判断是否需要log到文件
    if (app.log_qlen) {
        fprintf(
            app.qlen_file,
            "# %-10s %-8s %-8s %-8s\n",
            "<Time (in s)>",
            "<Port id>",
            "<Qlen in Bytes>",
            "<Buffer occupancy in Bytes>"
        );
        // 文件流缓冲区立即刷新,输出到文件
        fflush(app.qlen_file);
    }
    // 从大页内存分配一个工作mbuf,成功返回指针
    worker_mbuf = rte_malloc_socket(NULL, sizeof(struct app_mbuf_array),
            RTE_CACHE_LINE_SIZE, rte_socket_id());
    if (worker_mbuf == NULL)
        rte_panic("Worker thread: cannot allocate buffer space\n");

    // 如果force_quit不为真,那么i一直自加并对(端口数量-1)取余 比如四个口,i为0 1 2 3 0 1 2 3 ...
    for (i = 0; !force_quit; i = ((i + 1) & (app.n_ports - 1))) {
        int ret;

        /*ret = rte_ring_sc_dequeue_bulk(
            app.rings_rx[i],
            (void **) worker_mbuf->array,
            app.burst_size_worker_read);*/

        // 从ring中出队一个对象,非多消费者安全
        // ret_ring 指针,存放数据的指针数组,0为成功出队
        ret = rte_ring_sc_dequeue(
            app.rings_rx[i],
            (void **) worker_mbuf->array);

        // 没有数据,继续下一循环
        if (ret == -ENOENT)
            continue;

        // l2 learning
        // 给定的buf从头部开始,取一个type类型大小的对象,返回指针
        eth = rte_pktmbuf_mtod(worker_mbuf->array[0], struct ether_hdr*);

        // 根据source addr进行l2学习
        // 传入s-addr的指针,与之对应的端口号
        app_l2_learning(&(eth->s_addr), i);

        // l2 forward
        // 根据dst_addr查找目标端口
        dst_port = app_l2_lookup(&(eth->d_addr));
        if (unlikely(dst_port < 0)) {
            /* broadcast */
            RTE_LOG(DEBUG, SWITCH, "%s: broadcast packets\n", __func__);
            for (j = 0; j < app.n_ports; j++) {
                if (j == i) {
                    continue;
                } else if (j == (i ^ 1)) {
                    // 0-1 2-3 ..对应端口直接发送,进入队列
                    packet_enqueue(j, worker_mbuf->array[0]);
                } else {
                    // ???为什么这么操作,非对应端口需要复制
                    // 需要在buffer pool里面clone一个pkt
                    // 由于多播,需要复制一份计数,为间接buffer,函数将md的各结构体成员(除了引用计数)复制给mi
                    // 并将md的引用加一,mi->pkt.data指向md的data数据域
                    // 释放则根据引用计数来操作
                    new_pkt = rte_pktmbuf_clone(worker_mbuf->array[0], app.pool);
                    // 将新pkt进队
                    packet_enqueue(j, new_pkt);

                    // ???本来这段被注释掉,放到对应端口的tx_ring中
                    /*rte_ring_sp_enqueue(
                        app.rings_tx[j],
                        new_pkt
                    );*/
                }
            }
        } else {
            RTE_LOG(
                DEBUG, SWITCH,
                "%s: forward packet to %d--------\n",
                __func__, app.ports[dst_port]
            );
            packet_enqueue(dst_port, worker_mbuf->array[0]);
            /*rte_ring_sp_enqueue(
                app.rings_tx[dst_port],
                worker_mbuf->array[0]
            );*/
        }

        /*do {
            ret = rte_ring_sp_enqueue_bulk(
                app.rings_tx[i ^ 1],
                (void **) worker_mbuf->array,
                app.burst_size_worker_write);
        } while (ret < 0);*/
    }
}