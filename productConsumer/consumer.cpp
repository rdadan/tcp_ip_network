#include "consumer.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
using namespace dd;
Consumer::Consumer(TaskQueue &taskQue)
    : _taskQue(taskQue)
{
    cout << "Consumer()" << endl;
}
Consumer::~Consumer()
{
    cout << "~Consumer()" << endl;
}
void Consumer::run()
{
    ::srand(::time(NULL));
    int cnt = 5;
    while (cnt--)
    {
        Product product = _taskQue.popProduct();
        cout << "----- " << pthread_self() << " consumer " << product << endl;
        ::sleep(1);
    }
}
