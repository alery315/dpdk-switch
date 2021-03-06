//
// Created by alery on 2019/10/28.
//
#include "main.h"

#define DIFF(a,b) a > b ? a - b : 0

static void
log_qlen_info(uint32_t port_id) {
    uint32_t q, queues = app.n_queues;
    RTE_LOG(
            INFO, SWITCH,
            "%s: ------------------------------port is %u, bytes_buffer is %lu KiB, pkt_buffer is %lu\n",
            __func__, port_id,
            (app.qlen_bytes_in[port_id] - app.qlen_bytes_out[port_id]) / 1024,
            app.qlen_pkts_in[port_id] - app.qlen_pkts_out[port_id]
    );
    for (q = 0; q < queues; q++) {
        RTE_LOG(
                INFO, SWITCH,
                "%s: port is %u, queue is %u, bytes_buffer is %lu \n",
                __func__, port_id,
                q,
                (app.qlen_bytes_in_queue[port_id][q] - app.qlen_bytes_out_queue[port_id][q]) / 1024
        );
    }
}

static void
log_threshold(uint32_t port_id) {

    // 为了防止计数器爆表
    for (uint32_t j = 0; j < app.n_ports; ++j) {
        uint64_t d = app.qlen_bytes_out[j] / 2;
        if (app.qlen_bytes_in[j] > d) {
            app.qlen_bytes_in[j] -= d;
            app.qlen_bytes_out[j] = app.qlen_bytes_out[j] / 2;
            printf("switch: update app.qlen_bytes_out");
        } else {
            app.qlen_bytes_in[j] = app.qlen_bytes_out[j] + 128;
        }
        RTE_LOG(
                INFO, SWITCH,
                "%s: --------------qlen_in is > %lu, qlen_out is %lu, qlen_drop is %lu\n",
                __func__,
                app.qlen_bytes_in[j],
                app.qlen_bytes_out[j],
                app.qlen_drop[j]

        );
    }

    if (app.edt_policy) {
        RTE_LOG(
                INFO, SWITCH,
                "%s: -------------------------------unControl times is %u\n",
                __func__,
                app.unControlNums
        );
    }

    if (app.awa_policy) {
        uint32_t threshold;
        for (uint32_t i = 0; i < app.n_queues; ++i) {
            threshold = app.get_threshold(i);
            RTE_LOG(
                    INFO, SWITCH,
                    "%s: ---------------------------------------the threshold of queue %d is > %u, buffer occu is %lu\n",
                    __func__,
                    port_id,
                    threshold / 1024,
                    get_buff_occu_bytes() / 1024
            );
        }
        return;
    }
    uint32_t threshold = app.get_threshold(port_id);
    RTE_LOG(
            INFO, SWITCH,
            "%s: ----------------------------------------------------------------------------------------the threshold of port %d is > %u, buffer occu is %lu\n",
            __func__,
            port_id,
            threshold / 1024,
            get_buff_occu_bytes() / 1024
    );

    if (app.rl_policy) {
//        printf("port is %u, counter2_e is %ld, counter2_d is %ld, diff is %ld\n",
//               port_id,
//               app.counter2_e[port_id],
//               app.counter2_d[port_id],
//               app.counter2_e[port_id] - app.counter2_d[port_id]);

        for (uint32_t i = 0; i < app.n_queues; ++i) {
            printf("queue is %d, threshold is %ld\n", i, app.port_threshold[0][i]);
        }
        printf("********************************************************\n");
    }

}

static void
log_transmit_args(uint32_t port_id) {
    uint64_t tx_rate_scale = 0;
    uint64_t cpu_freq = rte_get_tsc_hz();
    uint64_t current_time, prev_time = app.prev_time[port_id];
    uint64_t tx_rate_bps;
    uint64_t target_tx_rate_bps;
    uint16_t n_mbufs;
    uint64_t token = app.token[port_id];
    // tsc_cycle / tsc_hz 就是秒为单位了
    tx_rate_scale = (((app.tx_rate_mbps >> 3) * (uint64_t) 1e6) << RATE_SCALE) / cpu_freq;
    tx_rate_bps = (tx_rate_scale * 8 * rte_get_tsc_hz()) >> RATE_SCALE;
    target_tx_rate_bps = app.tx_rate_mbps * (uint64_t) 1e6;
//    n_mbufs = app.mbuf_tx[port_id].n_mbufs;

    RTE_LOG(
            INFO, SWITCH,
            "%s: tx_rate_scale is %lu, current time is %lu, cpu_freq is %lu....\n",
            __func__, tx_rate_scale, current_time / cpu_freq, cpu_freq
    );

//    RTE_LOG(
//            INFO, SWITCH,
//            "%s: the n_mbufs of port %u is %u, ....\n",
//            __func__, port_id, n_mbufs
//    );

    current_time = rte_get_tsc_cycles();
    RTE_LOG(
            INFO, SWITCH,
            "%s: port is %u, before token is %lu, plus is %lu, bucket_size is %u....\n",
            __func__, port_id, token, (tx_rate_scale * (current_time - prev_time)) >> RATE_SCALE, app.bucket_size << 1
    );
    token += ((tx_rate_scale * (current_time - prev_time)) >> RATE_SCALE);
    token = MIN(token, (app.bucket_size << 1));
    RTE_LOG(
            INFO, SWITCH,
            "%s: port is %u, after token is %lu....\n",
            __func__, port_id, token
    );

}

void
app_main_loop_logging(void) {
    uint32_t i = 0;

//    while (!force_quit) {
//        log_threshold(0);
//        printf("i is %u\n", i++);
//        sleep(2);
//    }

    for (i = 0; !force_quit; i = ((i + 1) & (app.n_ports - 1))) {

        log_threshold(i);

//        log_qlen_info(i);

//        log_transmit_args(i);



        sleep(1);
    }
}



