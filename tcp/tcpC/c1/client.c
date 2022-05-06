#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

void do_service(int sockfd);

int main(int argc, const char *argv[])
{
    // 1. 创建套接字
    int peerfd = socket(PF_INET, SOCK_STREAM, 0);
    if (peerfd == -1)
        ERR_EXIT("socket");

    // 准备地址和端口号
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    // inet_addr把类似于201.211.214.36 转换为 4 字节的整数类型数据呢 ?
    // addr.sin_addr.s_addr = inet_addr("192.168.4.159");
    addr.sin_addr.s_addr = INADDR_ANY; // localhost
    addr.sin_port = htons(8888);
    socklen_t len = sizeof addr;

    // 2. 请求连接
    if (connect(peerfd, (struct sockaddr *)&addr, len) == -1) // 完成三次握手
        ERR_EXIT("Connect");

    char buf[1024];
    memset(buf, 0, sizeof(buf));
    read(peerfd, buf, sizeof(buf));
    printf("%s\n", buf);
    do_service(peerfd);

    return 0;
}

void do_service(int sockfd)
{
    char recvbuf[1024] = {0};
    char sendbuf[1024] = {0};
    while (1)
    {
        if (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
        {
            write(sockfd, sendbuf, strlen(sendbuf));
            memset(sendbuf, 0, sizeof(sendbuf));
        }

        // // read
        // int nread = read(sockfd, recvbuf, sizeof(recvbuf));
        // if (nread == -1)
        // {
        //     if (errno == EINTR)
        //         continue;
        //     ERR_EXIT("read");
        // }
        // else if (nread == 0)
        // {
        //     printf("server close!\n");
        //     close(sockfd);
        //     exit(EXIT_SUCCESS);
        // }
        // printf("receive msg : %s", recvbuf);
        // memset(recvbuf, 0, sizeof(recvbuf));
    }
}
