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
int64_t in_dims[] = {1, 8, 6, 8};
int input_dims = 8 * 6 * 8;
int one_input_dim = 6 * 8;
int out_put_dim = 8;
int64_t Trigger_number = 1000;
float last_threshold[8];
float buffer_size;

const char *file = "nn_model_ep_2060.pb";
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

    sleep(1);

    init(file);
    pre_run_session();

    buffer_size = (float)(app.buff_size_bytes / 60.0);

    uint32_t queues = app.n_queues;
    int dst_port = 0;
    int64_t pre_enqueue = 0;
    int64_t pre_drop = 0;
    int64_t pre_time = getCurrentTime();

    int position = 0;
    float values_queue[input_dims];
    for (int l = 0; l < input_dims; ++l) {
        values_queue[l] = 0;
    }

    for (uint32_t k = 0; k < queues; ++k) {
        last_threshold[k] = (float) app.buff_size_bytes / queues;
    }
    int64_t en_queue[queues];
    for (uint32_t m = 0; m < queues; ++m) {
        en_queue[m] = 0;
    }
    int64_t dropped_queue[queues];
    for (uint32_t n = 0; n < queues; ++n) {
        dropped_queue[n] = 0;
    }

    for (; !force_quit;) {

        int64_t diff = app.qlen_pkts_in[dst_port] + app.qlen_drop[dst_port] - pre_drop - pre_enqueue;
//        int64_t diff = 1100;

        if (diff >= Trigger_number) {
            int64_t time_interval = getCurrentTime() - pre_time;
//            RTE_LOG(
//                    INFO, SWITCH,
//                    "%s: time interval is %lu μs, diff is %lu packets\n",
//                    __func__, time_interval, diff
//            );

            int64_t trigger_number = Trigger_number;
            for (uint32_t i = 0; i < queues; ++i) {
                int64_t queue_length = app.qlen_pkts_in_queue[dst_port][i] - app.qlen_pkts_out_queue[dst_port][i];
                int64_t enqueue = app.qlen_pkts_in_queue[dst_port][i] - en_queue[i];
                en_queue[i] = app.qlen_pkts_in_queue[dst_port][i];
                int64_t dropped = app.qlen_drop_queue[dst_port][i] - dropped_queue[i];
                dropped_queue[i] = app.qlen_drop_queue[dst_port][i];
                float last_t = last_threshold[i];
                values_queue[position++] = (float)queue_length / buffer_size;
                values_queue[position++] = (float)enqueue / buffer_size;
                values_queue[position++] = (float)dropped / buffer_size;
                values_queue[position++] = (float)last_t;
                values_queue[position++] = (float)time_interval / 1000;
                values_queue[position++] = (float)trigger_number / buffer_size;
            }
            if (position >= input_dims) position -= input_dims;
//            printf("position is %d\n", position);
            int t_pos = position;

            float *values_p = malloc(sizeof(float) * input_dims);

            for (int j = 0; j < input_dims; ++j) {
                *(values_p + j) = values_queue[t_pos];
//                *(values_p + j) = (float) j;
//                printf("%8.4f ", *(values_p + j));
//                if (j % 6 == 5) {
//                    printf(" j: %d\n", j);
//                }

                t_pos++;
                if (t_pos >= input_dims) t_pos -= input_dims;
            }
//            printf("\n");

            pre_enqueue = app.qlen_pkts_in[dst_port];
            pre_drop = app.qlen_drop[dst_port];
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

//        if (diff <= -1) {
////            printf("trigger\n");
////            fflush(stdin);
//            int64_t time_interval = getCurrentTime() - pre_time;
////            int64_t trigger_number = Trigger_number;
////            for (uint32_t i = 0; i < queues; ++i) {
////                int64_t queue_length = app.qlen_pkts_in_queue[dst_port][i] - app.qlen_pkts_out_queue[dst_port][i];
////                int64_t enqueue = app.qlen_pkts_in_queue[dst_port][i] - en_queue[i];
////                en_queue[i] = app.qlen_pkts_in_queue[dst_port][i];
////                int64_t dropped = app.qlen_drop_queue[dst_port][i] - dropped_queue[i];
////                dropped_queue[i] = app.qlen_drop_queue[dst_port][i];
////                float last_t = last_threshold[i];
////                printf("queue is %d droped: %ld\n", i, dropped);
////                values_queue[position++] = (float)queue_length / buffer_size;
////                values_queue[position++] = (float)enqueue / buffer_size;
////                values_queue[position++] = (float)dropped / buffer_size;
////                values_queue[position++] = (float)last_t;
////                values_queue[position++] = (float)time_interval;
////                values_queue[position++] = (float)trigger_number / buffer_size;
////            }
////            if (position > input_dims) position -= input_dims;
////            int t_pos = position;
//
////            float *values_p = malloc(sizeof(float) * input_dims);
////
////            for (int j = 0; j < input_dims; ++j) {
//////                *(values_p + j) = values_queue[t_pos];
//////                printf("%8.4f ", *(values_p + j));
//////                t_pos++;
//////                if (j % 6 == 5) {
//////                    printf(" j: %d\n", j);
//////                }
//////                if (t_pos >= input_dims) t_pos -= input_dims;
////                *(values_p + j) = (float)j;
////            }
////            printf("\n");
//
////            pre_enqueue = app.qlen_pkts_in[dst_port];
////            pre_drop = app.qlen_drop[dst_port];
////            pre_time = getCurrentTime();
//            RTE_LOG(
//                    INFO, SWITCH,
//                    "%s: time interval is %lu, pre_enqueue is %lu, pre_drop is %lu\n",
//                    __func__, time_interval, pre_enqueue, pre_drop
//            );
////            run_session(values_p, (int)sizeof(float) * input_dims);
////            printf("quit \n");
////            fflush(stdin);
////            sleep(1);
//        }
//        printf("asdjfasfd \n");
//        fflush(stdin);
    }


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
        last_threshold[i] = ((*(out_values + i)) + 1) / 2;
        app.port_threshold[0][i] = (int64_t) (((*(out_values + i)) + 1) / 2.0 * buffer_size);
//        printf("queue is %d, threshold is %ld\n", i, app.port_threshold[0][i]);
    }
//    printf("\n");

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
    return (tv.tv_sec * 1000000) + (tv.tv_usec / 1);
}