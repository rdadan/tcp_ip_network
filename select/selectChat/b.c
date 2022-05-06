#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
//管道的写端
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("error args\n");
		return -1;
	}
	int fdw, fdr;
	fdw = open(argv[1], O_WRONLY); //必须O_WRONLY
	if (-1 == fdw)
	{
		perror("open");
		return -1;
	}
	fdr = open(argv[2], O_RDONLY); //必须O_RDONLY
	if (-1 == fdr)
	{
		perror("open");
		return -1;
	}
	printf("I am B\n");
	char buf[128];
	int ret;
	fd_set rdset;
	while (1)
	{
		FD_ZERO(&rdset);
		FD_SET(0, &rdset);
		FD_SET(fdr, &rdset);
		ret = select(fdr + 1, &rdset, NULL, NULL, NULL);
		if (ret > 0)
		{
			if (FD_ISSET(0, &rdset))
			{
				memset(buf, 0, sizeof(buf));
				ret = read(0, buf, sizeof(buf)); //读取标准输入
				if (0 == ret)
				{
					printf("byebye\n");
					close(fdr);
					close(fdw);
					break;
				}
				write(fdw, buf, strlen(buf) - 1); //写到1号管道
			}
			if (FD_ISSET(fdr, &rdset))
			{
				memset(buf, 0, sizeof(buf));
				ret = read(fdr, buf, sizeof(buf)); //从2号管道中读取数据
				if (0 == ret)
				{
					printf("byebye\n");
					close(fdr);
					close(fdw);
					break;
				}
				printf("%s\n", buf);
			}
		}
	}
	return 0;
}
