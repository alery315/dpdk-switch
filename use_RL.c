//
// Created by alery on 2019/11/28.
//
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <tensorflow/c/c_api.h>
#include <unistd.h>

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
int out_put_dim = 8;

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

static int64_t getCurrentTime();


void
app_main_loop_RL(void) {

    /* init and prepare run session */
    init(file);
    pre_run_session();

    for (int i = 0; i < 10; ++i) {
        /* this step reassign is necessary */
        float *values_p = malloc(sizeof(float) * input_dims);
        for (int j = 0; j < input_dims; ++j) {
            *(values_p + j) = (float) 3;
            printf("%f ", *(values_p + j));
        }
        printf("%p\n", values_p);

        /* run session */
        run_session(values_p, (int) sizeof(float) * input_dims);
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
    input = TF_NewTensor(TF_FLOAT, in_dims, n_dims, values_p, data_length, &deallocator, 0);

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
    printf("output value : ");
    for (int i = 0; i < out_put_dim; ++i) {
        printf("%f ", *(out_values + i));
    }
    printf("\n");

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
static int64_t
getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);    //该函数在sys/time.h头文件中
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}