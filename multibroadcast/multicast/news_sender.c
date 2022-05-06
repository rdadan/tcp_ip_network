#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 30
void error_handling(char *message);

// 多播是基于UDP套接字， 一般用于多媒体数据的实时传输
// 采用多播方式同时向多个套接字传递数据
// 加入多播主即可收到数据
// 多播组是D类IP地址（224.0.0.0 ~ 239.255.255.255）
int main(int argc, char *argv[])
{
    int send_sock;
    struct sockaddr_in mul_adr;
    // Time to Live 生存时间，每经过一个路由器就减一，等于0时不能数据包不能再传递，只能销毁
    int time_live = TTL;
    FILE *fp;
    char buf[BUF_SIZE];
    if (argc != 3)
    {
        printf("Usage : %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }
    send_sock = socket(PF_INET, SOCK_DGRAM, 0); //创建  UDP 套接字
    memset(&mul_adr, 0, sizeof(mul_adr));
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr(argv[1]); //必须将美女主播IP地址设置为多播地址
    mul_adr.sin_port = htons(atoi(argv[2]));
    //指定套接字中 TTL 的信息  IPPROTO_IP + IP_MULTICAST_TTL
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&time_live, sizeof(time_live));
    if ((fp = fopen("news.txt", "r")) == NULL)
        error_handling("fopen() error");

    while (!feof(fp)) //如果文件没结束就返回0
    {
        fgets(buf, BUF_SIZE, fp);
        // 本质是基于UDP套接字传输数据，所以使用sendto 和 recvfrom
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr *)&mul_adr, sizeof(mul_adr));
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