#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
//管道的读端
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("error args\n");
		return -1;
	}
	int fdr, fdw;
	fdr = open(argv[1], O_RDONLY); //必须O_RDONLY
	if (-1 == fdr)
	{
		perror("open");
		return -1;
	}
	fdw = open(argv[2], O_WRONLY); //必须O_WRONLY
	if (-1 == fdw)
	{
		perror("open");
		return -1;
	}
	printf("I am O_A\n");
	char buf[128] = {0};
	fd_set rdset;
	int ret;
	while (1)
	{
		FD_ZERO(&rdset);
		FD_SET(0, &rdset);
		FD_SET(fdr, &rdset);
		ret = select(fdr + 1, &rdset, NULL, NULL, NULL); //传入传出参数
		if (ret > 0)
		{
			if (FD_ISSET(fdr, &rdset)) //如果管道可读
			{
				memset(buf, 0, sizeof(buf));
				ret = read(fdr, buf, sizeof(buf)); //如果管道内没有数据，read会阻塞
				if (0 == ret)
				{
					printf("byebye\n");
					close(fdr);
					close(fdw);
					// break;
				}
				printf("%s\n", buf);
			}
			if (FD_ISSET(0, &rdset))
			{
				memset(buf, 0, sizeof(buf));
				ret = read(0, buf, sizeof(buf)); //读取标准输入,写到2号管道
				if (ret > 0)
				{
					write(fdw, buf, strlen(buf) - 1);
				}
				else
				{
					printf("byebye\n");
					close(fdr);
					close(fdw);
					break;
				}
			}
		}
	}
	return 0;
}
