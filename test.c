#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{

    char buff[1024];

    memset( buff, '\0', sizeof( buff ));

    fprintf(stdout, "启用全缓冲\n");
    setvbuf(stdout, buff, _IOFBF, 1024);

    fprintf(stdout, "这里是 runoob.com\n");
    fprintf(stdout, "该输出将保存到 buff\n");
    fflush( stdout );

    fprintf(stdout, "这将在编程时出现\n");
    fprintf(stdout, "最后休眠五秒钟\n");

    sleep(2);

    unsigned int a = 1;

    printf("%u\n", a << 2 * 4);

    char str[30] = "2030300test";
    char *ptr = NULL;
    long ret;

    ret = strtoul(str, &ptr, 10);
    printf("数字（无符号长整数）是 %lu\n", ret);
    printf("字符串部分是 %s\n", ptr);

    return(0);

    return(0);
}