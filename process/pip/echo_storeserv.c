#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int fds[2];

    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int str_len, state;
    char buf[BUF_SIZE];

    act.sa_handler = read_childproc; //防止僵尸进程
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);         //注册信号处理器,把成功的返回值给 state
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); //创建服务端套接字

    int reuse_addr = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

    // SO_REUSEPORT 实现系统级别的负载均衡,一个服务器程序可以在同一台物理主机的同一时刻启动多个
    int reuse_port = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port)) < 0;

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(8888);

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) //分配IP地址和端口号
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1) //进入等待连接请求状态
        error_handling("listen() error");

    pipe(fds);
    pid = fork();
    if (pid == 0)
    {
        while (1)
        {
            char msgbuf[BUF_SIZE];
            memset(msgbuf, 0, BUF_SIZE);
            int i, len;
            read(fds[0], msgbuf, BUF_SIZE);
            if (strcmp(msgbuf, "open") == 0)
            {
                FILE *fp = fopen("echomsg.txt", "wt");
                while ((len = read(fds[0], msgbuf, BUF_SIZE)))
                {
                    if (!strncmp(msgbuf, "q\n", strlen(msgbuf)) || !strncmp(msgbuf, "Q\n", strlen(msgbuf)))
                        break;
                    fwrite(msgbuf, sizeof(char), len, fp);
                    printf("msgbuf:%s, strlen: %d, len: %d\n", msgbuf, (int)strlen(msgbuf), len);
                    memset(msgbuf, 0, BUF_SIZE);
                }
                puts("fclose(fp)");
                fclose(fp);
            }
        }
    }
    while (1)
    {
        adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);
        if (clnt_sock == -1)
            continue;
        else
        {
            write(fds[1], "open", 4);
            puts("new client connected...");
        }
        pid = fork(); //此时，父子进程分别带有一个套接字
        if (pid == 0) //子进程运行区域,此部分向客户端提供回声服务
        {
            close(serv_sock); //关闭服务器套接字，因为从父进程传递到了子进程
            while ((str_len = read(clnt_sock, buf, BUFSIZ)) != 0)
            {
                write(clnt_sock, buf, str_len);
                write(fds[1], buf, str_len);
                memset(buf, 0, BUF_SIZE);
            }
            write(fds[1], "q", 1);
            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        else
            close(clnt_sock); //通过 accept 函数创建的套接字文件描述符已经复制给子进程，因为服务器端要销毁自己拥有的
    }
    close(serv_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}
