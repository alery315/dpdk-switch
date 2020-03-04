#include <signal.h>

#include "main.h"

volatile bool force_quit;


/**
 * 处理中断信号,强制退出改为true
 * @param signum
 */
static void
signal_handler(int signum) {
    switch (signum) {
        case SIGTERM:
        case SIGINT:
            RTE_LOG(
                    INFO, SWITCH,
                    "%s: Receive %d signal, prepare to exit...\n",
                    __func__, signum
            );
            force_quit = true;
            break;
    }
}

/**
 * App退出操作,停止端口,关闭端口,(可选:关闭log文件)
 */
static void
app_quit(void) {
    uint8_t i;
    /* close ports */
    for (i = 0; i < app.n_ports; i++) {
        uint8_t port = (uint8_t) app.ports[i];
        RTE_LOG(
                INFO, SWITCH,
                "%s: Closing NIC port %u ...\n",
                __func__, port
        );
        rte_eth_dev_stop(port);
        rte_eth_dev_close(port);
    }
    /* free resources */
    /*if (app_cfg.cfg != NULL) {
        cfg_free(app_cfg.cfg);
    }
    if (app_cfg.bm_policy != NULL) {
        free(app_cfg.bm_policy);
    }
    if (app_cfg.qlen_fname != NULL) {
        free(app_cfg.qlen_fname);
    }*/
    /* close files */
    if (app.log_qlen) {
        fclose(app.qlen_file);
    }
    printf("App quit. Bye...\n");
}

int
main(int argc, char **argv) {

    uint32_t lcore;
    int ret;

    /* Init EAL */
    ret = rte_eal_init(argc, argv);
    if (ret < 0)
        return -1;

    // ???
    argc -= ret;
    argv += ret;

//    for (int i = 0; i < argc; i++) {
//        printf("the %d arg is %s\n", i, argv[i]);
//    }


    force_quit = false;
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Parse application arguments (after the EAL ones) */
    ret = app_parse_args(argc, argv);
    if (ret < 0) {
        app_print_usage();
        return -1;
    }

    /* Init */
    app_init();

    app.start_cycle = rte_get_tsc_cycles();

    /* Launch per-lcore init on every lcore */
    rte_eal_mp_remote_launch(app_lcore_main_loop, NULL, CALL_MASTER);

    // 一个for循环,除了master core都会执行,执行等待每个lcore变成wait状态
    RTE_LCORE_FOREACH_SLAVE(lcore) {
        if (rte_eal_wait_lcore(lcore) < 0) {
            return -1;
        }
    }

    app_quit();
    fflush(stdout);

    return 0;
}

int
app_lcore_main_loop(__attribute__((unused)) void *arg) {
    unsigned lcore, i;

    // 返回正在运行的核
    lcore = rte_lcore_id();


    if (lcore == app.core_rx) {
        RTE_LOG(
                INFO, SWITCH,
                "%s: current lcore is %u, doing main loop rx...\n",
                __func__, lcore
        );
        app_main_loop_rx();
        return 0;
    }


    if (lcore == app.core_worker) {
        RTE_LOG(
                INFO, SWITCH,
                "%s: current lcore is %u, doing main loop forwarding...\n",
                __func__, lcore
        );
        app_main_loop_forwarding();
        return 0;
    }

    if (app.rl_policy) {
        for (i  = 0; i < app.n_ports; ++i) {
            if (lcore == app.core_rl[i]) {
                RTE_LOG(
                        INFO, SWITCH,
                        "%s: current lcore is %u, doing rl calc for port %d...\n",
                        __func__, lcore, i
                );
                app_main_loop_RL(i);
                return 0;
            }
        }
    }

//    if (lcore == app.core_rl && app.rl_policy) {
//        RTE_LOG(
//                INFO, SWITCH,
//                "%s: current lcore is %u, doing rl calc ...\n",
//                __func__, lcore
//        );
//        app_main_loop_RL();
//        return 0;
//    }

    if (lcore == app.core_log) {
        RTE_LOG(
                INFO, SWITCH,
                "%s: current lcore is %u, doing debug logging ...\n",
                __func__, lcore
        );
        app_main_loop_logging();
        return 0;
    }

    // 如果给定的核够每个端口分配一个(被rx与forward占用两个)
    if (app.n_lcores >= 2 + app.n_ports) {

        for (i = 0; i < app.n_ports; i++) {
            if (lcore == app.core_tx[i]) {
                RTE_LOG(
                        INFO, SWITCH,
                        "%s: current lcore is %u, doing main loop tx, port is %u ...\n",
                        __func__, lcore, i
                );
                app_main_loop_tx_each_port(i);
                return 0;
            }
        }
    } else {
        // 假如不够,只用了core_tx的第一个核
        if (lcore == app.core_tx[0]) {
            app_main_loop_tx();
            return 0;
        }
    }

    return 0;
}
