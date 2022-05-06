#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// 单向同信：子进程只负责写， 父进程只负责读
void test01()
{
    int fds[2];
    pipe(fds); //创建管道 fds[1]写端 fds[0]读端
    pid_t pid;
    pid = fork();
    char buf[128] = "hello";
    if (pid == 0)
    {
        //子进程往管道里写
        write(fds[1], buf, sizeof(buf));
    }
    else
    {
        // 父进程读
        read(fds[0], buf, 128);
        puts(buf);
    }
}

// 一条管道实现 双向向同信：子进程只负责先写后读， 父进程先读后写
void test02()
{
    int fds[2];
    pipe(fds); //创建管道 fds[1]写端 fds[0]读端
    pid_t pid;
    pid = fork();
    char buf[128] = "hello";
    if (pid == 0)
    {
        write(fds[1], buf, sizeof(buf));
        // 这里如果不挂起，子进程会立马把管道的数据读出来，导致父进程读不到数据，会一直阻塞等待管道有数据
        sleep(2);
        read(fds[0], buf, 128);
        printf("child read buf: %s\n", buf);
    }
    else
    {
        // 父进程读
        read(fds[0], buf, 128);
        printf("parent read buf: %s\n", buf);
        write(fds[1], buf, sizeof(buf));
        sleep(3); // 这里睡觉是等待子进程，不然父进程就直接退出了，而子进程还在运行
    }
}
// test02()说明 一条管道实现双向通信，有缺陷
// 用两个管道实现双向通信
void test03()
{
    int fds1[2], fds2[2];
    pipe(fds1); //创建管道 fds[1]写端 fds[0]读端
    pipe(fds2); //创建管道 fds[1]写端 fds[0]读端
    pid_t pid;
    pid = fork();
    char buf[128] = "hello helo";
    if (pid == 0)
    {

        // 子进程用1号管道写段
        close(fds1[0]);
        write(fds1[1], buf, sizeof(buf));
        // 子进程只用2号管道读段
        close(fds2[1]);
        memset(buf, 0, 128);
        read(fds2[0], buf, 128);
        printf("child read buf: %s\n", buf);
    }
    else
    {
        // 父进程用1号管道读段
        close(fds1[1]);
        memset(buf, 0, 128);
        read(fds1[0], buf, 128);
        printf("parent read buf: %s\n", buf);
        // 父进程只用1号管道写段
        close(fds2[0]);
        write(fds2[1], buf, sizeof(buf));
    }
}

int main()
{
    test03();
    return 0;
}