///
/// @file    server.cc
/// @author  lemon(haohb13@gmail.com)
/// @date    2016-03-22 10:13:03
///

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

// 1. 第一步：调用 socket 函数创建套接字。
// 2. 第二步：调用 bind 函数分配IP地址和端口号。
// 3. 第三步：调用 listen 函数转换为可接受请求状态。
// 4. 第四步：调用 accept 函数受理套接字请求。
void process_err(int fd, char *message)
{
	perror(message);
	close(fd);
	exit(EXIT_FAILURE);
}
int main(void)
{
	// 1. 创建套接字
	int server_fd = socket(AF_INET, SOCK_STREAM, 0); //
	if (-1 == server_fd)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

#if 1
	// SO_REUSEADDR 一个服务器程序主动关闭之后，立马可以重启
	int reuse_addr = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0)
	{
		process_err(server_fd, "setsockopt reuse_addr error");
	}
#endif

#if 1
	// SO_REUSEPORT 实现系统级别的负载均衡,一个服务器程序可以在同一台物理主机的同一时刻启动多个
	int reuse_port = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port)) < 0)
	{
		process_err(server_fd, "setsockopt reuse_port error");
	}
#endif

#if 1
	// 传输大文件 建议禁用Nagle算法（A给B发送数据， 等收到B的ack之后，再发下一条数据）
	// 大文件一般每次传输都会占满缓冲区， 即使不使用Nagle算法， 也会在装满输出缓冲区时传输数据包
	// 禁用Nagle算法，可以在无需等待ACK的前提下连续传输
	int tcp_nodelay = 1;
	setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, (void *)&tcp_nodelay, sizeof(tcp_nodelay));
#endif
	// 准备地址 端口号
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(struct sockaddr_in));
	serverAddr.sin_family = AF_INET;   //小端模式      大端模式
	serverAddr.sin_port = htons(8888); //主机字节序转成网络字节序列
	// serverAddr.sin_addr.s_addr = INADDR_ANY; //代表本机地址

	// 2. 将listenfd和地址、端口号进行绑定
	int ret = bind(server_fd,
				   (struct sockaddr *)&serverAddr,
				   sizeof(struct sockaddr));
	if (-1 == ret)
	{
		process_err(server_fd, "bind");
	}

	// 3. 准备在套接字listenfd上接受连接
	ret = listen(server_fd, 15);
	if (-1 == ret)
	{
		process_err(server_fd, "listen");
	}

	// 4. 接收请求，成功时返回新的文件描述符，失败时返回-1
	int client_fd = accept(server_fd, NULL, NULL); // 完成三次握手
	char buff[1024] = "welcome to server1..";	   //用户态的应用缓冲区
	ret = write(client_fd, buff, strlen(buff));	   //将应用缓冲区的数据直接copy到内核
	memset(buff, 0, strlen(buff) + 1);
	strcpy(buff, "welcome to server2.");		//用户态的应用缓冲区
	ret = write(client_fd, buff, strlen(buff)); //将应用缓冲区的数据直接copy到内核
	while (1)
	{
		printf("> client: ");

		char recvbuf[1024] = {0};
		ret = read(client_fd, recvbuf, 1024); //将内核缓冲区中的数据直接copy到应用缓冲区
		if (ret == -1)
		{
			if (errno == EINTR)
				continue;
		}
		else if (ret == 0)
		{
			printf("client close!\n");
			break;
		}
		if (ret > 0)
		{
			printf("%s\n", recvbuf);
		}
	}
	close(server_fd);
	close(client_fd);
	return 0;
}
