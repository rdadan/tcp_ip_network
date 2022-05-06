#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

int main()
{
    //创建文件描述符
    fd_set fd_read, fd_temp;
    //所有文件描述符初始化为0
    FD_ZERO(&fd_read);
    // 0号位置置位1
    FD_SET(0, &fd_read); // 0是标准输入
    struct timeval timeout;

    while (1)
    {
        fd_temp = fd_read;
        timeout.tv_sec = 3;
        timeout.tv_usec = 3;
        // 1只监听一个描述符，就是标准输入
        int res = select(1, &fd_temp, 0, 0, &timeout);
        if (res == -1)
        {
            perror("select");
            break;
        }
        else if (res == 0)
        {
            puts("time out");
        }
        else
        {
            char buf[32] = {};
            int len = read(0, buf, 32); // 从标准输入读
            printf("get input msg: %s", buf);
        }
    }
    return 0;
}