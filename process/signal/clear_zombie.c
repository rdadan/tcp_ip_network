#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
// 使用sigaction和waitpid回收僵尸进程
// 如果子进程结束，发送信号，在信号处理函数中使用waitpid回收子进程

void clear_child(int sig)
{
    puts("clear_child");
    id_t status;
    // -1表示可以等待任意p子进程终止， WNOHANG表示技术即使没有子进程终止也不尽然阻塞状态
    // 成功返回子进程id，失败返回-1
    pid_t pid = waitpid(-1, &status, WNOHANG);
    //如果正常退出，WIFEXITED()返回TURE
    if (WIFEXITED(status))
    {
        printf("---------------clear child pid: %d\n", pid);
        printf("---------------child return status val: %d\n", WEXITSTATUS(status));
    }
}

int main()
{

    //注册信号
    struct sigaction sigact;
    sigact.sa_handler = clear_child;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGCHLD, &sigact, 0);

    pid_t pid;

    pid = fork();
    if (pid == 0)
    {
        printf("I am child, pid: %d, I will return 5 after 5s\n\n", getpid());
        sleep(5);
        return 5;
    }
    else
    {
        printf("I am parent, pid: %d \n\n", getpid());
        pid = fork();
        if (pid == 0)
        {
            printf("I am new child, pid: %d, I will return 10 after 10s\n\n", getpid());
            sleep(10);
            exit(10);
        }
        else
        {
            printf("I am parent, pid: %d \n\n", getpid());
            for (int i = 0; i < 5; i++)
            {
                puts("parent wait 5s...");
                sleep(5);
            }
        }
    }

    return 0;
}