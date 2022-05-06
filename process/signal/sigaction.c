#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    if (sig == SIGALRM)
    {
        printf("TIME OUT 01!\n");
    }
}

void test01()
{
    // int sigaction (int __sig, const struct sigaction *__restrict __act,struct sigaction *__restrict __oact)
    struct sigaction sigact;
    sigact.sa_handler = timeout;
    // sigact.sa_flags = 0;
    // sigemptyset(&sigact.sa_mask);
    sigaction(SIGALRM, &sigact, 0);
    alarm(2);
    sleep(100);
}

int main()
{
    test01();
    return 0;
}