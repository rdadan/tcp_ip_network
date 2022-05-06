#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(const char *message);

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    int str_len = 0;
    int idx = 0, read_len = 0;
    char message[30];
    // server只用了一次write，发来“Hello world!”， client可以分多次read
    while (read_len = read(sock, &message[idx++], 1)) // read_len = 0时 证明读到末尾
    {
        printf("read_len: %d\n", read_len);
        if (read_len == -1)
            error_handling("read() error");

        str_len += read_len;
    }

    printf("message from server： %s\n", message);
    printf("Function read call count: %d\n", str_len);

    close(sock);

    return 0;
}

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
