/*
   面向连接的套接字(SOCK_STREAM)

   1.传输过程中数据不会消失
   2.按序传输数据
   3.传输的数据不存在数据边界

   总结:可靠的、按序传递的、基于字节的面向连接的数据传输方式


   面向消息的套接字(SOCK_DGRAM)

   1.强调快速传输而非传输顺序
   2.传输的数据可能丢失也可能损毁
   3.传输的数据有数据边界
   4.限制每次传输的数据大小

   总结:不可靠的、不按序传递的、以数据的高速传输为目的的套接字
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(const char *message);

int main(int argc, char const *argv[])
{
	int serv_sock;
	int clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	// if (argc != 2)
	// {
	// 	printf("Usage : %s <port>\n", argv[0]);
	// 	exit(1);
	// }

	serv_sock = socket(PF_INET, SOCK_STREAM, 0); //创建套接字
	if (serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	// serv_addr.sin_addr.s_addr = inet_addr("172.25.104.244"); // htonl(INADDR_ANY);
	// serv_addr.sin_port = htons(2222);						 // htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(8888);

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) //调用bind函数分配ip和端口
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1) //调用listen函数将套接字转为可接收连接状态
		error_handling("listen() error");

	clnt_addr_size = sizeof(clnt_addr);

	//调用accept函数受理连接请求,如果在没有连接的情况下调用该函数,则不会返回,直到有连接请求为止.
	//意思是这里会一直阻塞,直到有连接请求
	clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
	if (clnt_sock == -1)
		error_handling("accept() error");

	//打印连接上来的客户端ip port
	printf("server ip:%s,port:%d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
	printf("client ip:%s,port:%d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
	char message[] = "Hello world!";

	write(clnt_sock, message, sizeof(message));
	close(clnt_sock);
	close(serv_sock);

	return 0;
}

void error_handling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
