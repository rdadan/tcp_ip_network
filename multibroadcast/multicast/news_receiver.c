#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;
    // 多播主结构体
    struct ip_mreq join_adr;
    if (argc != 3)
    {
        printf("Usage : %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }
    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[2]));

    if (bind(recv_sock, (struct sockaddr *)&adr, sizeof(adr)) == -1)
        error_handling("bind() error");
    //初始化结构体
    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]); //多播组地址（美女主播地址）
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);  //待加入多播主的主机IP地址 （猥琐男地址）
    //利用套接字选项 IP_ADD_MEMBERSHIP 加入多播组，完成了接受指定的多播组数据的所有准备
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&join_adr, sizeof(join_adr));

    while (1)
    {
        // 本质是基于UDP套接字传输数据，所以使用recvfrom
        //如果不需要知道传输数据的主机地址信息，可以向recvfrom函数的第5 6参数分贝传入 NULL 0
        str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if (str_len < 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }
    close(recv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}