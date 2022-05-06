#include <stdio.h>
#include <unistd.h>

int gVal = 10;
int main()
{
    pid_t pid;
    int lVal = 20;
    gVal++;     // 11
    lVal += 15; // 35
    pid = fork();
    if (pid == 0) //子进程
    {
        gVal += 2; // 11+2=13
        lVal += 2; // 35+2 = 37
    }
    else // 父进程
    {
        gVal -= 2; // 11-2=9
        lVal -= 2; // 35-2 = 33;
    }

    // parent pross: gVal: 9, Lva: 33
    // child pross: gVal: 13, Lva: 37
    // fork之后区分父子进程，互补影响， 只是共用了一份代码
    if (pid == 0)
    {
        printf("child pross: gVal: %d, Lva: %d \n", gVal, lVal);
    }
    else
    {
        printf("parent pross: gVal: %d, Lva: %d \n", gVal, lVal);
    }
    return 0;
}