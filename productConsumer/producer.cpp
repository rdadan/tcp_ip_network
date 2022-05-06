#include "producer.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
using namespace dd;
Producer::Producer(TaskQueue &taskQue)
    : _taskQue(taskQue)
{
    cout << "Producer()" << endl;
}
Producer::~Producer()
{
    cout << "~Producer()" << endl;
}
void Producer::run()
{
    ::srand(::time(NULL));
    int cnt = 5;
    while (cnt--)
    {
        Product product = ::rand() % 100;
        cout << "----- " << pthread_self() << " produce " << product << endl;
        _taskQue.pushProduct(product);
        ::sleep(2);
    }
}
