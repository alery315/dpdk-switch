#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

int64_t getCurrentTime()      //直接调用这个函数就行了，返回值最好是int64_t，long long应该也可以
{
    struct timeval tv;
    gettimeofday(&tv, NULL);    //该函数在sys/time.h头文件中
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void thread(int thread) {
    int i;
    for (i = 0; i < 3; i++)
        printf("This is a pthread. time is %ld thread is %d\n", getCurrentTime(), thread);
}

int main() {

//    char buff[1024];
//
//    memset( buff, '\0', sizeof( buff ));
//
//    fprintf(stdout, "启用全缓冲\n");
//    setvbuf(stdout, buff, _IOFBF, 1024);
//
//    fprintf(stdout, "这里是 runoob.com\n");
//    fprintf(stdout, "该输出将保存到 buff\n");
//    fflush( stdout );
//
//    fprintf(stdout, "这将在编程时出现\n");
//    fprintf(stdout, "最后休眠五秒钟\n");
//
//    sleep(2);
//
//    unsigned int a = 1;
//
//    printf("%u\n", a << 2 * 4);
//
//    char str[30] = "2030300test";
//    char *ptr = NULL;
//    long ret;
//
//    ret = strtoul(str, &ptr, 10);
//    printf("数字（无符号长整数）是 %lu\n", ret);
//    printf("字符串部分是 %s\n", ptr);

//    struct timeval start, end;
//    int a = 2,b = 3;
//    gettimeofday( &start, NULL );
//    for (long i = 0; i < 999999; i++) {
//        if((a & b)) {
//
//        }
//    }
//    sleep(1);
//    gettimeofday( &end, NULL );
//    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
//    printf("time: %d us\n", timeuse);
//
//    gettimeofday( &start, NULL );
//    sleep(1);
//    for (long i = 0; i < 999999; i++) {
//        if(a < b) {
//
//        }
//    }
//    gettimeofday( &end, NULL );
//    timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec -start.tv_usec;
//    printf("time: %d us\n", timeuse);
//
//    printf("%ld\n", getCurrentTime());
//    sleep(1);
//    printf("%ld\n", getCurrentTime());

    pthread_t id[4];
    int i, ret;

    // 成功返回0，错误返回错误编号
    for (i = 0; i < 4; i++) {
        ret = pthread_create(&id[i], NULL, (void *) thread, (void *)i);
        if (ret != 0) {
            printf("Create pthread error!\n");
            exit(1);
        }
    }


    for (i = 0; i < 3; i++)
        printf("This is main process. time is %ld\n", getCurrentTime());

    for (i = 0; i < 4; i++) {
        pthread_join(id[i], NULL);
    }

    printf("time is %ld\n", getCurrentTime());
    for (int j = 0; j < 99999999; ++j) {
        getCurrentTime();
    }
    printf("time is %ld\n", getCurrentTime());



    return 0;
}
