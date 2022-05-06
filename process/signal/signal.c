
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout01(int sig)
{
    if (sig == SIGALRM)
    {
        printf("TIME OUT 01!\n");
    }
    alarm(3); // 每 3s产生一个SIGALRM信号，会激活timeout(int sig)函数
}
void timeout02(int sig)
{
    if (sig == SIGALRM)
    {
        printf("TIME OUT 02!\n");
    }
    alarm(3); // 每 3s产生一个SIGALRM信号，会激活timeout(int sig)函数
}
void keycontrol(int sig)
{
    if (sig == SIGINT)
    {
        puts("KEY CONTROL!");
    }
}

void test01()
{
    // 由alarm函数发出信号
    signal(SIGALRM, timeout01);

    // 按ctrl c 激活信号
    signal(SIGINT, keycontrol);
    // 预约2s后产生一个SIGALRM信号，激活timeout(int sig)函数
    alarm(2);

    for (int i = 0; i < 3; i++)
    {
        puts("wait ...");
        // 发生信号时，会立马唤醒因为sleep函数而进入阻塞状态的进程，
        // 所以一定发生信号，这里就不会睡100s
        sleep(100);
    }
}

void test02()
{
    void (*pSingal)(int);
    // signal() 回上一次处理这个信号的处理函数的地址
    pSingal = signal(SIGALRM, timeout01);
    alarm(2);
    sleep(100);

    pSingal = signal(SIGALRM, timeout02);
    alarm(2);
    sleep(100);
    pSingal(66); // 执行 timeout02
    sleep(100);
}

int main()
{
    test02();
    return 0;
}