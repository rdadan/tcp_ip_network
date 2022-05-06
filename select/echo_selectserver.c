#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#define buf_size 1024

void handle_err(int fd, char *msg)
{
    perror(msg);
    close(fd);
    exit(EXIT_FAILURE);
}
/*
在I/O编程过程中，当需要同时处理多个客户端接入请求时，可以利用多线程或者I/O多路复用技术进行处理。
    I/O多路复用技术通过把多个I/O的阻塞复用到同一个select的阻塞上，
    从而使得系统在单线程的情况下可以同时处理多个客户端请求。
    与传统的多线程/多进程模型比，I/O多路复用的最大优势是系统开销小，
    系统不需要创建新的额外进程或者线程，也不需要维护这些进程和线程的运行，降底了系统的维护工作量，节省了系统资源。

I/O多路复用的主要应用场景如下：
    服务器需要同时处理多个处于监听状态或者多个连接状态的套接字。
    服务器需要同时处理多种网络协议的套接字。
*/

/*
select函数返回产生事件的文件描述符数量，如果为-1则出错，为 0 则超时。
值得注意的是，比如用户态要监听描述符1和3的读事件，则将readset对应bit置为1，
当调用select函数之后，若只有1描述符就绪，则readset对应bit为1，但是描述符3对应的位置为0，
这就需要注意，每次调用select的时候，都需要重新初始化并赋值readset结构体，
将需要监听的描述符对应的bit置为1，而不能直接使用readset，因为这个时候readset已经被内核改变了。

优点：
1. select多路复用模型是单进程可为多个客户端服务，这样可减少创建进程和线程所需要的CPU时间片和内存等资源的消耗
缺点:
1.每次调用select函数都会把 fd_set 结合中的用户拷贝到内核态，之后内核需要遍历所传进来的 fd ，如果客户端数量较多时，会导致系统开销变大。
2.单进程能够监听文件描述符数量有限，在linux上一般为1024个，可通过 setrlimit() 去修改宏定义甚至重新编译内核来提升这一限制，但也会对效率有所影响。
3.select采用的是触发方式为水平触发，应用程序如果没有完成对一个已就绪的文件描述符进行I/O操作那么每次select调用还是会将文件描述符通知进程。

原文链接：https://blog.csdn.net/weixin_42647166/article/details/104830425
*/
int main()
{

    // 1 socket
    int server_sock;
    int ret = 0;
    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    // SO_REUSEADDR 一个服务器程序主动关闭之后，立马可以重启
    int reuse_addr = 1;
    ret = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
    if (ret < 0)
    {
        handle_err(server_sock, "setsockopt reuse_addr error");
    }

    // SO_REUSEPORT 实现系统级别的负载均衡,一个服务器程序可以在同一台物理主机的同一时刻启动多个
    int reuse_port = 1;
    ret = setsockopt(server_sock, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port)) < 0;
    if (ret < 0)
    {
        handle_err(server_sock, "setsockopt reuse_port error");
    }

    // 2 bind
    struct sockaddr_in ser_addr;
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ser_addr.sin_port = htons(8888);
    ser_addr.sin_family = AF_INET;

    ret = bind(server_sock, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
    if (ret == -1)
    {
        handle_err(server_sock, "bind");
    }

    // 3 listen
    ret = listen(server_sock, 10);
    if (ret == -1)
    {
        handle_err(server_sock, "listen");
    }

    // 4 accept
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buf[buf_size] = {};

    // 5 select, fd_read 事件数组
    fd_set fd_read, cp_read;
    FD_ZERO(&fd_read);
    FD_SET(server_sock, &fd_read); // fd_read变量注册服务器端套接字server_fd
    int fd_max = server_sock;
    struct timeval timeout;
    while (1)
    {
        timeout.tv_sec = 3;
        timeout.tv_usec = 5;
        // Returns the number of ready descriptors
        // 每次调用select的时候，都需要重新初始化并赋值readset结构体
        cp_read = fd_read;
        // 第二个参数是读时间，第三个参数是写事件，第三个参数是异常事件
        // 缺点1：每次调用select都要传递监视对象的信息
        int ready_num = select(fd_max + 1, &cp_read, 0, 0, &timeout);
        if (ready_num == -1)
        {
            perror("select");
            break;
        }
        if (ready_num == 0)
        {
            printf("time out ,server_sock: %d\n", server_sock);
            continue;
        }
        // 缺点2：针对所有文件描述符进行循环 ；优点:兼容好；使用较少的客户端
        for (int fd_idx = 0; fd_idx < fd_max + 1; fd_idx++)
        {
            //判断当前循环到的套接字文件描述符是否有事件
            if (FD_ISSET(fd_idx, &cp_read))
            {
                // 如果是服务端套接字有变化，说明有新的客户端连接
                if (fd_idx == server_sock) // server_sock==3; 0是表示输入(read)，1是标准输出(write)，2是标准错误
                {
                    // client_sock 从4开始
                    addr_len = sizeof(client_addr);
                    client_sock = accept(fd_idx, (struct sockaddr *)&client_addr, &addr_len);
                    printf("client connect, fd_max:%d, fd_idx:%d, cli_sock:%d\n", fd_max, fd_idx, client_sock);
                    // 每来一个新的客户端，都注册新客户端的套接字。后续调用 select 函数时无需重复注册
                    FD_SET(client_sock, &fd_read);
                    if (client_sock > fd_max) // 随着cli连接bianduo， fd_max也增大
                    {
                        fd_max = client_sock;
                    }
                }
                //不是服务端套接字时，处理客户端请求，这时  fd_idx == client_sock
                else
                {
                    // // fd_idx指的是当前发起请求的客户端
                    // ret = read(client_sock, buf, buf_size);
                    // if (ret == 0)
                    // {
                    //     // 客户端退出，i号事件清除
                    //     FD_CLR(client_sock, &fd_read);
                    //     close(client_sock);
                    //     printf("client exit, fd_idx:%d, cli_sock:%d\n", fd_idx, client_sock);
                    // }
                    // else
                    // {
                    //     printf("receive from client: %s, fd_idx:%d, cli_sock:%d\n", buf, fd_idx, client_sock);
                    //     memset(buf, 0, sizeof(buf));
                    //     write(client_sock, buf, ret);
                    // }
                    // fd_idx指的是当前发起请求的客户端
                    // 这里不能使用 while (ret = read(fd_idx, buf, buf_size))
                    // 水平触发, 应用程序如果没有完成对一个已就绪的文件描述符进行I/O操作
                    // 那么每次select调用还是会将文件描述符通知进程。
                    // 假如每次读一个字节，不是buf_size，一个事件就会触发多次
                    ret = read(fd_idx, buf, 1);
                    if (ret == 0)
                    {
                        // 客户端退出，i号事件清除
                        FD_CLR(fd_idx, &fd_read);
                        close(fd_idx);
                        printf("client exit, fd_idx:%d, cli_sock:%d\n", fd_idx, client_sock);
                    }
                    else
                    {
                        // long num = 3000000000;
                        // while (num--)
                        // {
                        // }
                        printf("receive from client: %s, fd_max:%d, fd_idx:%d, cli_sock:%d\n", buf, fd_max, fd_idx, client_sock);
                        memset(buf, 0, sizeof(buf));
                        write(fd_idx, buf, ret);
                    }
                }
            }
        }
    }
    close(server_sock);
    return 0;
}