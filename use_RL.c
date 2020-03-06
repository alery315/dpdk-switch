//
// Created by alery on 2019/11/28.
//
#include "main.h"

/* ------------------Global Variable----------------- */

TF_Buffer *graph_buf;
TF_Graph *graph;
TF_Status *status;
TF_ImportGraphDefOptions *graph_opts;
TF_SessionOptions *sess_opts;
TF_Session *session;

// input
TF_Operation *input_op;
TF_Output input_opout;
TF_Tensor *input;

// operation
const TF_Operation *target_op;
const TF_Operation *const *target_opers;

// output
TF_Output output;
TF_Output *outputs;
TF_Tensor *output_values;

// input data
int n_dims = 4;
int64_t in_dims[] = {1, 8, 6, 4};
int input_dims = 8 * 6 * 4;
int one_input_dim = 6 * 4;
int out_put_dim = 4;
int64_t trigger_number = 20;
float last_threshold[4];
float buffer_size;
int upper = 9;
int lower = -6;

// output file
int64_t pre_time_file = 0;
char *rl_file_name = "rl_log.txt";
FILE *rl_file;
volatile bool log_info;

const char *file = "nn_model_ep_10.pb";
const char *input_op_name = "Placeholder";
const char *operation_name = "main/mul";


TF_Buffer *read_file(const char *file);

void init(const char *file);

void pre_run_session();

static void run_session(float *values_p, int data_length);

void check_status_ok(TF_Status *status, char *step);

void free_buffer(void *data, size_t length);

static void deallocator(void *data, size_t length, void *arg);

//static uint64_t getCurrentTime();


void
app_main_loop_RL(void) {

    /* init and prepare run session */

//    sleep(1);

    init(file);
    pre_run_session();

    buffer_size = (float)(app.buff_size_bytes / 1.0);

    uint64_t sum_pkts_in = 0;
    uint64_t sum_pkts_out = 0;
    uint64_t sum_pkts_drop = 0;
    uint32_t ports = app.n_ports;
    int64_t pre_enqueue = 0;
    int64_t pre_dequeue = 0;
    int64_t pre_drop = 0;
    int64_t pre_time = getCurrentTime();

    int position = 0;
    float values_queue[input_dims];
    for (int l = 0; l < input_dims; ++l) {
        values_queue[l] = 0;
    }

    for (uint32_t k = 0; k < ports; ++k) {
        last_threshold[k] = 1;
    }
    int64_t en_queue[ports];
    for (uint32_t m = 0; m < ports; ++m) {
        en_queue[m] = 0;
    }
    int64_t dropped_queue[ports];
    for (uint32_t n = 0; n < ports; ++n) {
        dropped_queue[n] = 0;
    }

    for (; !force_quit;) {

//        int64_t diff = app.qlen_pkts_in[dst_port] + app.qlen_drop_bytes[dst_port] - app.qlen_bytes_out[dst_port] - pre_drop -
//                       pre_enqueue + pre_dequeue;
        sum_pkts_in = 0;
        sum_pkts_out = 0;
        sum_pkts_drop = 0;
        for (uint32_t k = 0; k < ports; ++k) {
            sum_pkts_in += app.qlen_pkts_in[k];
            sum_pkts_out += app.qlen_pkts_out[k];
            sum_pkts_drop += app.qlen_drop[k];
        }
        int64_t diff = sum_pkts_in + sum_pkts_drop - sum_pkts_out - pre_enqueue
                        - pre_drop + pre_dequeue;
//        int64_t diff = 1100;

        if (diff >= trigger_number) {
            int64_t time_interval = getCurrentTime() - pre_time;
//            RTE_LOG(
//                    INFO, SWITCH,
//                    "%s: time interval is %lu μs, diff is %lu packets\n",
//                    __func__, time_interval, diff
//            );

            for (uint32_t i = 0; i < ports; ++i) {
                // 组装数据
                // 队列长度
                int64_t queue_length = app.qlen_bytes_in[i] - app.qlen_bytes_out[i];
                // 经过了多少bytes
                int64_t enqueue = app.qlen_bytes_out[i] - en_queue[i];
                en_queue[i] = app.qlen_bytes_out[i];
                // 丢了多少bytes
                int64_t dropped = app.qlen_drop_bytes[i] - dropped_queue[i];
                dropped_queue[i] = app.qlen_drop_bytes[i];
                // 上次返回值
                float last_t = last_threshold[i];
                values_queue[position++] = (float)queue_length / buffer_size;
                values_queue[position++] = (float)enqueue / buffer_size;
                values_queue[position++] = (float)dropped / buffer_size;
                values_queue[position++] = (float)last_t;
                values_queue[position++] = (float)time_interval / buffer_size;
                values_queue[position++] = (float)trigger_number;
            }
            if (position >= input_dims) position -= input_dims;
//            printf("position is %d\n", position);
            int t_pos = position;

            float *values_p = malloc(sizeof(float) * input_dims);

            for (int j = 0; j < input_dims; ++j) {
                *(values_p + j) = values_queue[t_pos];

//                *(values_p + j) = (float) j;
                //输出结果
//                printf("%8.4f ", *(values_p + j));
//                if (j % 6 == 5) {
//                    printf(" j: %d\n", j);
//                }

                t_pos++;
                if (t_pos >= input_dims) t_pos -= input_dims;
            }
//            printf("\n");

            pre_enqueue = sum_pkts_in;
            pre_dequeue = sum_pkts_out;
            pre_drop = sum_pkts_drop;
            pre_time = getCurrentTime();

            /* run session to get result */
            run_session(values_p, (int)sizeof(float) * input_dims);


//            pre_enqueue = app.qlen_pkts_in[dst_port];
//            pre_drop = app.qlen_drop[dst_port];
//            pre_time = getCurrentTime();
//            RTE_LOG(
//                    INFO, SWITCH,
//                    "%s: qlen_pkts_in is %ld, qlen_drop is %ld, diff is %ld, time interval is %ld\n",
//                    __func__, app.qlen_pkts_in[dst_port] - pre_enqueue, app.qlen_drop[dst_port] - pre_drop, diff,
//                    time_interval
//            );

        }

        if (diff <= -1) {
            int64_t time_interval = getCurrentTime() - pre_time;
        }

    }


    // log file
//    free(rl_file_name);
    fclose(rl_file);
    log_info = false;

    // free resource
    TF_CloseSession(session, status);
    TF_DeleteSession(session, status);
    TF_DeleteSessionOptions(sess_opts);
    TF_DeleteImportGraphDefOptions(graph_opts);
    TF_DeleteGraph(graph);
    TF_DeleteStatus(status);
    TF_DeleteBuffer(graph_buf);



}


/**
 * use .pb file to init a tf_buffer and return it
 * @param t_file
 * @return
 */
TF_Buffer *
read_file(const char *t_file) {
    FILE *f = fopen(t_file, "rb");
    fseek(f, 0, SEEK_END);
    long f_size = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    void *data = malloc(f_size);
    fread(data, f_size, 1, f);
    fclose(f);

    TF_Buffer *buf = TF_NewBuffer();
    buf->data = data;
    buf->length = f_size;
    buf->data_deallocator = free_buffer;
    return buf;
}

/**
 * init work: init graph, status, session
 * @param t_file
 */
void
init(const char *t_file) {
    // Use read_file to get graph_def as TF_Buffer*
    graph_buf = read_file(t_file);
    graph = TF_NewGraph();

    // Import graph_def into graph
    status = TF_NewStatus();
    graph_opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, graph_buf, graph_opts, status);

    check_status_ok(status, "import graph");

    sess_opts = TF_NewSessionOptions();
    session = TF_NewSession(graph, sess_opts, status);

    check_status_ok(status, "create new session");

    // file
    rl_file = fopen(rl_file_name, "a+");
    if (rl_file == NULL) {
        perror("Open file error:");
        RTE_LOG(
                ERR, SWITCH,
                "%s: Cannot open rl log file '%s'\n",
                __func__, rl_file_name
        );
    }

    fprintf(
        rl_file,
        "%-8s %-8s %-8s %-8s %-10s %-10s\n",
        "<Port 1>",
        "<Port 2>",
        "<Port 3>",
        "<Port 4>",
        "<interval>",
        "<Time(ms)>");
    // 文件流缓冲区立即刷新,输出到文件
    fflush(rl_file);

}

/**
 * prepare to run session: get input and operation name
 */
void
pre_run_session() {
    // Pass the graph and a string name of your input operation
    // (make sure the operation name is correct)
    // input
    input_op = TF_GraphOperationByName(graph, input_op_name);
    input_opout.oper = input_op;
    input_opout.index = 0;

    // operation
    target_op = TF_GraphOperationByName(graph, operation_name);
    target_opers = &target_op;
    // output
    output.oper = (void *) target_op;
    output.index = 0;
    outputs = &output;

    check_status_ok(status, "prepare work for run session");
}

/**
 * use session and status to run DL, paras is point to input_values and its length
 * @param values_p
 * @param data_length
 */
static void
run_session(float *values_p, int data_length) {

    log_info = true;

    // Create the input tensor using the dimension (in_dims) and size (num_bytes_in)
    // variables created earlier
    input = TF_NewTensor(TF_FLOAT, in_dims, n_dims, values_p, data_length, &deallocator, NULL);

    TF_SessionRun(
            session,
            NULL,
            &input_opout, &input, 1,
            outputs, &output_values, 1,
            target_opers, 1,
            NULL,
            status
    );
    // get result after run session
    float *out_values = TF_TensorData(output_values);

//    printf("output value : \n");
    for (int i = 0; i < out_put_dim; ++i) {
//        printf("%f ", *(out_values + i));
//        last_threshold[i] = ((*(out_values + i)) + 1) / 2;
        last_threshold[i] = *(out_values + i);
        app.port_alpha[i] = (int32_t) (((*(out_values + i)) + 1) / 2.0 * (upper - lower + 1)) + lower;
//        printf("queue is %d, alpha is %d\n", i, app.port_alpha[i]);
    }
//    printf("\n");

    // log to file
    fprintf(
            rl_file,
            "%-8d %-8d %-8d %-8d %-10lu %-10lu\n",
            // 记录的cpu_freq主要用在这里
            app.port_alpha[0],
            app.port_alpha[1],
            app.port_alpha[2],
            app.port_alpha[3],
            getCurrentTime() - pre_time_file,
            getCurrentTime()
    );
    fflush(rl_file);
    pre_time_file = getCurrentTime();

    // 这里不能free,会导致后面TF_NewTensor时候空指针
//     free(values_p);
}


void
free_buffer(void *data, size_t length) {
    free(data);
}


static void
deallocator(void *data, size_t length, void *arg) {
    free(data);
    // *reinterpret_cast<bool*>(arg) = true;
}


/**
 * check status ok
 * @param status
 * @param step
 */
void
check_status_ok(TF_Status *t_status, char *step) {
    if (TF_GetCode(t_status) != TF_OK) {
        fprintf(stderr, "Error at step \"%s\", status is: %u\n", step, TF_GetCode(t_status));
        fprintf(stderr, "Error message: %s\n", TF_Message(t_status));
        exit(EXIT_FAILURE);
    } else {
        printf("%s\n", step);
    }
}


/* 直接调用这个函数就行了,返回值最好是int64_t*/
int64_t
getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);    //该函数在sys/time.h头文件中
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}