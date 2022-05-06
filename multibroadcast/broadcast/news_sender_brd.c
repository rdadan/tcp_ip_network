#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
// 广播和多播一样，都是一次性向多个主机发送数据， 也是基于UDP完成的
// 多播即使在跨越不同的网络， 只要加入广播组，就能接收数据
//广播只能向同一网络中的主机发送数据
int main(int argc, char *argv[])
{
    int send_sock;
    struct sockaddr_in broad_adr;
    FILE *fp;
    char buf[BUF_SIZE];
    if (argc != 3)
    {
        printf("Usage : %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }
    send_sock = socket(PF_INET, SOCK_DGRAM, 0); //创建  UDP 套接字
    memset(&broad_adr, 0, sizeof(broad_adr));
    broad_adr.sin_family = AF_INET;
    broad_adr.sin_addr.s_addr = inet_addr(argv[1]);
    broad_adr.sin_port = htons(atoi(argv[2]));
    int so_brd = 1;
    // 只需将SO_BROADCAST选项置为1
    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void *)&so_brd, sizeof(so_brd));
    if ((fp = fopen("news.txt", "r")) == NULL)
        error_handling("fopen() error");

    while (!feof(fp)) //如果文件没结束就返回0
    {
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr *)&broad_adr, sizeof(broad_adr));
        sleep(2);
    }
    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}