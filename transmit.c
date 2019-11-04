#include "main.h"

static uint64_t u64_diff(uint64_t x, uint64_t y) {
    return (x > y ? (x-y) : (y-x));
}

/**
 * 根据port id初始化
 * @param port_id
 */
static void init_tx(int port_id) {
    uint32_t lcore = rte_lcore_id();
    // 每秒时钟中断的次数
    uint64_t cpu_freq = rte_get_tsc_hz();
    // 从启动后的tsc次数
    uint64_t current_time = rte_get_tsc_cycles();
    uint64_t tx_rate_scale = 0;
    uint64_t tx_rate_bps, target_tx_rate_bps;

    // (tx_rate_mbps / 8 * 1e6) * 2^20 / cpu_freq

    tx_rate_scale = (((app.tx_rate_mbps >> 3) * (uint64_t)1e6) << RATE_SCALE) / cpu_freq;
    app.cpu_freq[lcore] = cpu_freq;
    app.prev_time[port_id] = current_time;
    app.token[port_id] = app.bucket_size;
    app.core_tx[port_id] = lcore;
    app.tx_rate_scale[port_id] = tx_rate_scale;
    tx_rate_bps = (app.tx_rate_scale[port_id] * 8 * rte_get_tsc_hz())>>RATE_SCALE;
    target_tx_rate_bps = app.tx_rate_mbps * (uint64_t)1e6;
    RTE_LOG(
        INFO, SWITCH,
        "%s: actual tx_rate of port %u: %lubps=%luMbps\n",
        __func__,
        app.ports[port_id],
        tx_rate_bps,
        tx_rate_bps/(uint64_t)1e6
    );
    // 计算差值
    if (u64_diff(tx_rate_bps, target_tx_rate_bps) > target_tx_rate_bps/20) {
        RTE_LOG(
            ERR, SWITCH,
            "%s: Calculated tx_rate(%lubps) is significantly different from origin tx rate(%lubps). Integer overflow?\n",
            __func__,
            tx_rate_bps, target_tx_rate_bps
        );
    }
}

/**
 * 只有一个核做tx的情况
 */
void
app_main_loop_tx(void) {
    uint32_t i;

    RTE_LOG(INFO, SWITCH, "Core %u is doing TX\n", rte_lcore_id());

    for (i = 0; i < app.n_ports; i++) {
        init_tx(i);
    }
    for (i = 0; !force_quit; i = ((i + 1) & (app.n_ports - 1))) {
        app_main_tx_port(i);
    }
}

/**
 * 为每个port分配一个core
 * @param port_id
 */
void
app_main_loop_tx_each_port(uint32_t port_id) {

    RTE_LOG(INFO, SWITCH, "Core %u is doing TX for port %u\n", rte_lcore_id(), app.ports[port_id]);
    init_tx(port_id);
    while(!force_quit) {
        app_main_tx_port(port_id);
    }
}

void app_main_tx_port(uint32_t port_id) {
    struct rte_mbuf* pkt;
    uint64_t current_time, prev_time = app.prev_time[port_id];
    // tx_rate的放大倍数
    uint64_t tx_rate_scale = app.tx_rate_scale[port_id];
    uint16_t n_mbufs, n_pkts;
    uint64_t token = app.token[port_id];
    int ret;

    // 当前端口含有多少数据指针
    n_mbufs = app.mbuf_tx[port_id].n_mbufs;

    // 一直都是0?
    if (n_mbufs > 0) {
        RTE_LOG(
                DEBUG, SWITCH,
                "%s: %u port has %u packets *************\n",
                __func__, app.ports[port_id], n_mbufs
        );
    }

    current_time = rte_get_tsc_cycles();
    // 配置值
    if (app.tx_rate_mbps > 0) {
        // tbf: generate tokens
        token += ((tx_rate_scale * (current_time - prev_time)) >> RATE_SCALE);
        token = MIN(token, (app.bucket_size<<1));

        app.prev_time[port_id] = current_time;
        if (token < app.bucket_size) {
            app.token[port_id] = token;
            return ;
        }
    }
    // 从tx_ring出队一个对象
    ret = rte_ring_sc_dequeue(
        app.rings_tx[port_id],
        (void **) &app.mbuf_tx[port_id].array[n_mbufs]);

    if (ret == -ENOENT) { /* no packets in tx ring */
        return ;
    }

    /**
     * 出队成功,更新记录参数
     *
     */
    pkt = app.mbuf_tx[port_id].array[n_mbufs];
    app.qlen_bytes_out[port_id] += pkt->pkt_len;
    app.qlen_pkts_out[port_id] ++;
    if (app.tx_rate_mbps > 0) {
        token -= pkt->pkt_len;
        app.token[port_id] = token;
    }

    n_mbufs ++;

//    RTE_LOG(
//        INFO, SWITCH,
//        "%s: port %u receive %u packets\n",
//        __func__, app.ports[port_id], n_mbufs
//    );

    // 如果当前port的包小于burst_size_tx_write则return
    if (n_mbufs < app.burst_size_tx_write) {
        app.mbuf_tx[port_id].n_mbufs = n_mbufs;
        return ;
    }

    uint16_t k = 0;
    do {
        n_pkts = rte_eth_tx_burst(
            app.ports[port_id],
            0,
            &app.mbuf_tx[port_id].array[k],
            n_mbufs - k);
        k += n_pkts;
//        if (k < n_mbufs) {
//            RTE_LOG(
//                INFO, SWITCH,
//                "%s: Transmit ring is full in port %u\n",
//                __func__, app.ports[port_id]
//            );
//        }
//        RTE_LOG(
//                INFO, SWITCH,
//                "%s: port %u k is %u\n",
//                __func__, app.ports[port_id], k
//        );
    } while (k < n_mbufs);

    app.mbuf_tx[port_id].n_mbufs = 0;
}
