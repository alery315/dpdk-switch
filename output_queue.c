#include "main.h"

#define u_diff(a,b) ((a > b) ? (a - b) : 64)


// 固定分配策略
uint32_t
qlen_threshold_equal_division(uint32_t port_id) {
    // port_id * 2 ???
    port_id = port_id << 1; /* prevent warning */
    uint32_t result = app.buff_size_bytes / app.n_ports;
    return result;
}

uint32_t
qlen_threshold_cs(uint32_t port_id) {
    port_id = port_id << 1; /* prevent warning */
//    printf("complete sharing %u\n",app.buff_size_bytes);
    return app.buff_size_bytes;
}

// 根据DT分配策略
uint32_t
qlen_threshold_dt(uint32_t port_id) {
    port_id = port_id << 1; /* prevent warning */
    // (总大小 - 已占buf) * alpha
    return ((app.buff_size_bytes - get_buff_occu_bytes()) << app.dt_shift_alpha);
}

uint32_t
qlen_threshold_edt(uint32_t port_id){
    if (app.isUnControl[port_id]) {
        uint8_t n_unControl = 0;
        for (uint32_t i = 0; i < app.n_ports; i++) {
            if(app.isUnControl[i])
                n_unControl++;
        }
//        printf("port %u is uncontrol, threshold is %u\n", port_id, app.buff_size_bytes / n_unControl);
        return app.buff_size_bytes / n_unControl;
    }
    return ((app.buff_size_bytes - get_buff_occu_bytes()) << app.dt_shift_alpha);
}

uint32_t
qlen_threshold_rl(uint32_t port_id) {
    // (总大小 - 已占buf) * alpha
    if (app.port_alpha[port_id] < 0) {
        return ((app.buff_size_bytes - get_buff_occu_bytes()) >> -app.port_alpha[port_id]);
    }
    return ((app.buff_size_bytes - get_buff_occu_bytes()) << app.port_alpha[port_id]);
}

uint32_t
qlen_threshold_awa(uint32_t queue_id) {
    return ((app.buff_size_bytes - get_buff_occu_bytes()) * app.priority_alpha[queue_id]);
}

// 已经进来 - 已经出去 = 还在输出队列中的占用着buf的
uint64_t get_qlen_bytes(uint32_t port_id) {
    return u_diff(app.qlen_bytes_in[port_id], app.qlen_bytes_out[port_id]);
}

// 获得总buf占用,byte为单位
uint64_t get_buff_occu_bytes(void) {
    uint64_t result = 0;
    uint32_t i;
    for (i = 0; i < app.n_ports; i++) {
//        result += (app.qlen_bytes_in[i] - app.qlen_bytes_out[i]);
        result += u_diff(app.qlen_bytes_in[i],app.qlen_bytes_out[i]);
    }
    return result;
    //return app.buff_bytes_in - app.buff_bytes_out;
}

//static int mark_packet_with_ecn(struct rte_mbuf *pkt) {
//    struct ipv4_hdr *iphdr;
//    uint16_t cksum;
//    if (RTE_ETH_IS_IPV4_HDR(pkt->packet_type)) {
//        iphdr = rte_pktmbuf_mtod_offset(pkt, struct ipv4_hdr *, sizeof(struct ether_hdr));
//        if ((iphdr->type_of_service & 0x03) != 0) {
//            iphdr->type_of_service |= 0x3;
//            iphdr->hdr_checksum = 0;
//            cksum = rte_ipv4_cksum(iphdr);
//            iphdr->hdr_checksum = cksum;
//        } else {
//            return -2;
//        }
//        return 0;
//    } else {
//        return -1;
//    }
//}

/**
 * 数据包进入队列
 * 目标端口,通用数据包指针
 * @param dst_port
 * @param pkt
 * @return ret 执行结果值
 * 0 packet enqueue to port
 * -1 Packet dropped due to queue length > threshold
 * -2 Packet dropped due to buffer overflow
 * -3 Cannot mark packet with ECN, drop packet
 */
int packet_enqueue(uint32_t dst_port, struct rte_mbuf *pkt) {
    int ret = 0;
    int mark_pkt = 0, mark_ret;

    // 获取目标端口的queue长度,bytes为单位,in - out,就是还在交换机中的数据,占用了缓存的数据量
    uint32_t qlen_bytes = get_qlen_bytes(dst_port);

    // 定义阈值
    uint32_t threshold = 0;
    // queue长度+new_pkt长度 pkt_len = 60
    uint32_t qlen_enque = qlen_bytes + pkt->pkt_len;
    // 已占用的总buf容量
    uint32_t buff_occu_bytes = 0;
    // 标记新包 << 10 把本来kb单位的转化为byte单位
    // 开启ecn并且qlen大于threshold ???
//    mark_pkt = (app.ecn_enable && qlen_bytes >= (app.ecn_thresh_kb<<10));
    /*Check whether buffer overflows after enqueue*/
    if (app.shared_memory) {
        // 所有的 in - out, buf占用
        // ------------???这个地方是否需要换到下面去执行,因为中间几行代码有执行时延--------
        buff_occu_bytes = get_buff_occu_bytes();

        // get_threshold 获得阈值回调函数,可以根据配置文件读取,equal division 或者 DT
//        if (app.rl_policy) {
//            threshold = app.port_threshold[dst_port];
//            // 由于这两个是端口级别的,所以需要把qlen_enque转成端口级别判断
////            threshold = app.get_threshold(dst_queue);
////            qlen_enque = app.qlen_bytes_in_queue[dst_port][dst_queue] - app.qlen_bytes_out_queue[dst_port][dst_queue];
////            qlen_enque += pkt->pkt_len;
//        } else {
//            threshold = app.get_threshold(dst_port);
//        }

        threshold = app.get_threshold(dst_port);

        // 大于阈值了 或者 大于总buf长度了
        if (qlen_enque > threshold) {
            ret = -1;
        } else if (buff_occu_bytes + pkt->pkt_len > app.buff_size_bytes) {
            ret = -2;
        }
    } else if (qlen_enque > app.buff_size_per_port_bytes) { // 非共享内存,端口buf独占且均分
        ret = -2;
    }

//    // ECN marking
//    if (ret == 0 && mark_pkt) {
//        /* do ecn marking */
//        mark_ret = mark_packet_with_ecn(pkt);
//        if (mark_ret < 0) {
//            ret = -3;
//        }
//        /* end */
//    }

    // ret为0,表示正常情况,可以进入队列,否则丢包,free掉这个packet
    if (ret == 0) {
        // 添加到目标队列的ring_tx中
        int enqueue_ret = rte_ring_sp_enqueue(
            app.rings_tx[dst_port],
            pkt
        );
        if (enqueue_ret != 0) {
//            RTE_LOG(
//                ERR, SWITCH,
//                "%s: packet cannot enqueue in port %u queue %u, due to no enough room in the ring\n",
//                __func__, app.ports[dst_port], dst_queue
//            );
            // 假如ring满了而导致没有进队，那么返回等待吧
            return ret;
//            rte_pktmbuf_free(pkt);
        } else {
//            RTE_LOG(
//                    DEBUG, SWITCH,
//                    "%s: packet enqueue in port %u queue %u\n",
//                    __func__, app.ports[dst_port], dst_queue
//            );
            app.qlen_bytes_in[dst_port] += pkt->pkt_len;
//            app.qlen_bytes_in_queue[dst_port][dst_queue] += pkt->pkt_len;
            // 更新输出队列 in pkt
            app.qlen_pkts_in[dst_port]++;
//            app.qlen_pkts_in_queue[dst_port][dst_queue] ++;
            // 改变优先级
//            app.queue_priority[dst_port] = dst_queue > app.queue_priority[dst_port] ? dst_queue : app.queue_priority[dst_port];

            /* enqueue */
            if (app.edt_policy) {

                app.counter1[dst_port] = 0;

//                if (!app.flag[dst_port]) {
//                    app.counter1[dst_port] = 0;
//                }

                if (app.flag[dst_port]) {
                    app.counter2_e[dst_port] = 0;
                    app.counter2_d[dst_port] = 0;
                } else {
                    app.counter2_e[dst_port]++;
                }

//                printf("------------------------------------------port %u counter2_e is %u\n", dst_port,
//                       app.counter2_e[dst_port]);


                if (app.counter2_e[dst_port] - app.counter2_d[dst_port] >= app.C2) {
                    app.time2[dst_port] = rte_get_tsc_cycles();
                    app.isUnControl[dst_port] = 1;
                    app.unControlNums++;
//                    printf("*********************%u is uncontrol****************\n", dst_port);
                }

//                if (app.counter1[dst_port] == app.C1) {
//                    app.isUnControl[dst_port] = 0;
//                }

                if (app.isUnControl[dst_port] && rte_get_tsc_cycles() - app.time2[dst_port] > app.scale_max_burst_time) {
                    app.isUnControl[dst_port] = 0;
//                    printf("*********************%u is control time expired****************\n", dst_port);
                }

                // 假如出队列不更新状态,是否会锁死直到下一个包进来
                if (app.flag[dst_port] || app.isUnControl[dst_port]) {
                    app.time1[dst_port] = rte_get_tsc_cycles();
                }

                app.flag[dst_port] = rte_get_tsc_cycles() - app.time1[dst_port] > app.scale_T1 || app.isUnControl[dst_port];


            }


//            app.queue_priority[dst_port] &= dst_queue;
            /*app.buff_bytes_in += pkt->pkt_len;
            app.buff_pkts_in ++;*/
        }
        // 更新输出队列 in bytes
//        RTE_LOG(
//                DEBUG, SWITCH,
//                "%s: current port is %u, the pkt_len is %u ..........\n",
//                __func__, app.ports[dst_port], pkt->pkt_len
//        );

        /**
         * 这一段log_qlen,没用
        if (app.log_qlen && pkt->pkt_len >= MEAN_PKT_SIZE &&
            (app.log_qlen_port >= app.n_ports ||
             app.log_qlen_port == app.ports[dst_port])
        ) {
            if (app.qlen_start_cycle == 0) {
                app.qlen_start_cycle = rte_get_tsc_cycles();
            }

//             "<Time (in s)>",
//             "<Port id>",
//             "<Qlen in Bytes>",
//             "<Buffer occupancy in Bytes>"

            fprintf(
                app.qlen_file,
                "%-12.6f %-8u %-8u %-8u\n",
                 // 记录的cpu_freq主要用在这里
                (float) (rte_get_tsc_cycles() - app.qlen_start_cycle) / app.cpu_freq[rte_lcore_id()],
                app.ports[dst_port],
                qlen_bytes,
                buff_occu_bytes
            );
        }
        */
    } else {

        // EDT policy
        if (app.edt_policy) {
            app.counter2_e[dst_port] = 0;
            app.counter2_d[dst_port] = 0;
        }
//        printf("drop one packet");
        app.qlen_drop[dst_port]++;
        app.qlen_drop_bytes[dst_port] += pkt->pkt_len;
//        app.qlen_drop_queue[dst_port][dst_queue]++;
        //释放掉这个包
        rte_pktmbuf_free(pkt);

    }
    switch (ret) {
    case 0:
        RTE_LOG(
            DEBUG, SWITCH,
            "%s: packet enqueue to port %u\n",
            __func__, app.ports[dst_port]
        );
        break;
    case -1:
        RTE_LOG(
            DEBUG, SWITCH,
            "%s: Packet dropped due to queue length > threshold\n",
            __func__
        );
        break;
    case -2:
        RTE_LOG(
            DEBUG, SWITCH,
            "%s: Packet dropped due to buffer overflow\n",
            __func__
        );
    case -3:
        RTE_LOG(
            DEBUG, SWITCH,
            "%s: Cannot mark packet with ECN, drop packet\n",
            __func__
        );
    }
    return ret;
}
