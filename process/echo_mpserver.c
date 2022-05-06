#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define buf_size 1024

void handle_err(int fd, char *msg)
{
    perror(msg);
    close(fd);
    exit(EXIT_FAILURE);
}

void clear_child(int sig)
{
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

    // 1 socket
    int server_fd;
    int ret = 0;
    server_fd = socket(PF_INET, SOCK_STREAM, 0);
    // SO_REUSEADDR 一个服务器程序主动关闭之后，立马可以重启
    int reuse_addr = 1;
    ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
    if (ret < 0)
    {
        handle_err(server_fd, "setsockopt reuse_addr error");
    }

    // SO_REUSEPORT 实现系统级别的负载均衡,一个服务器程序可以在同一台物理主机的同一时刻启动多个
    int reuse_port = 1;
    ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port)) < 0;
    if (ret < 0)
    {
        handle_err(server_fd, "setsockopt reuse_port error");
    }

    // 2 bind
    struct sockaddr_in ser_addr;
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ser_addr.sin_port = htons(8888);
    ser_addr.sin_family = AF_INET;

    ret = bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
    if (ret == -1)
    {
        handle_err(server_fd, "bind");
    }

    // 3 listen
    ret = listen(server_fd, 10);
    if (ret == -1)
    {
        handle_err(server_fd, "listen");
    }

    // 4 accept
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buf[buf_size] = {};

    // a
    struct sigaction act;
    act.sa_handler = clear_child; //防止僵尸进程
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    //注册信号处理器,把成功的返回值给 state
    sigaction(SIGCHLD, &act, 0);
    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1)
        {
            continue;
        }

        puts("client connected...");

        // 创建子进程, 此时，父子进程分别带有一个套接字
        pid_t pid = fork();
        if (pid == -1)
        {
            close(client_fd);
            continue;
        }
        if (pid == 0) // 进入子进程区域
        {
            close(server_fd); //关闭服务器套接字，因为从父进程传递到了子进程
            printf("child %d got a client \n", getpid());
            // 读输入缓冲区
            while ((ret = read(client_fd, buf, buf_size)) != 0)
            {
                printf("receive from client: %s\n", buf);
                memset(buf, 0, sizeof(buf));
                write(client_fd, buf, ret);
            }
            close(client_fd);
            printf("child %d return, because client disconnected\n", getpid());
            return 0;
        }
        else
        {
            //通过 accept 函数创建的套接字文件描述符已经复制给子进程，所以服务器端要销毁自己拥有的client_fd
            close(client_fd);
        }
    }
    close(server_fd);
    return 0;
}