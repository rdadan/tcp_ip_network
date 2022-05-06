#include "taskQueue.h"
#include <iostream>
using std::cout;
using std::endl;
using namespace dd;

TaskQueue::TaskQueue(size_t queSize)
    : _queSize(queSize),
      _mutex(),
      _condNotEmpty(_mutex),
      _condNotFull(_mutex)
{
}
size_t TaskQueue::getQueSize()
{
    return _queSize;
}
bool TaskQueue::isEmpty()
{
    return _que.size() == 0;
}
bool TaskQueue::isFull()
{
    return _que.size() == _queSize;
}
void TaskQueue::pushProduct(Product product)
{
    AutoMutex autoMutex(_mutex);
    // 队列满 线程等待。用while是为了防止异常唤醒
    //虚假唤醒：线程从pthread_cond_wait()返回了，但原本线程期望被唤醒时的条件(not full)并不满足。
    while (isFull())
    {
        // 等待不满的信号
        cout << " que isFull" << endl;
        _condNotFull.condWait();
    }
    _que.push(product);
    // 发送不空的信号
    _condNotEmpty.condNotify();
}
Product TaskQueue::popProduct()
{
    AutoMutex autoMutex(_mutex);
    // 队列空就阻塞等待
    while (isEmpty())
    {
        // 等待不空的信号
        cout << " que isEmpty" << endl;
        _condNotEmpty.condWait();
    }
    Product product = _que.front();
    _que.pop();
    // 发送不满的信号
    _condNotFull.condNotify();
    return product;
}
