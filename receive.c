#include "main.h"

void
app_main_loop_rx(void) {
    uint32_t i;
    int ret;

    RTE_LOG(INFO, SWITCH, "Core %u is doing RX\n", rte_lcore_id());

    // cpu_freq[lcore] = cpu自启动后的运行时钟周期
    app.cpu_freq[rte_lcore_id()] = rte_get_tsc_hz();
    // 如果force_quit不为真,那么i一直自加并对(端口数量-1)取余 比如四个口,i为0 1 2 3 0 1 2 3 ...
    for (i = 0; !force_quit ; i = ((i + 1) & (app.n_ports - 1))) {
        uint16_t n_mbufs;

        /**
         * 从网络设备端口取回数据到指定的rte_mbuf指针数组
         * port_id,queue_id,rx_pkt指针数组的地址,需要读的数量
         * return:返回实际取回的数量
         */
        n_mbufs = rte_eth_rx_burst(
            app.ports[i],
            0,
            app.mbuf_rx.array,
            app.burst_size_rx_read);

        // 取回的buf数量>=给定的数量 默认64,打一个log
        if (n_mbufs >= app.burst_size_rx_read) {
            RTE_LOG(
                INFO, SWITCH,
                "%s: receive %u packets from port %u\n",
                __func__, n_mbufs, app.ports[i]
            );
        }

        if (n_mbufs == 0)
            continue;

        if (n_mbufs > 0) {
            RTE_LOG(
                    DEBUG, SWITCH,
                    "%s: receive %u packets from port %u------------------\n",
                    __func__, n_mbufs, app.ports[i]
            );
        }

        do {
            // 把几个对象放到ring中
            // ring的指针,几个对象的指针,从几个对象中取出并加入ring的数量,操作后ring剩余空间
            // return value : 0 or n 进队的数量
            ret = rte_ring_sp_enqueue_bulk(
                app.rings_rx[i],
                (void **) app.mbuf_rx.array,
                n_mbufs, NULL);
            RTE_LOG(
                    DEBUG, SWITCH,
                    "%s: enqueue %u packets into %u's rings_rx---------------------\n",
                    __func__, ret, app.ports[i]
            );
        } while (ret == 0);
    }
}
