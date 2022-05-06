#include "thread.h"
using namespace dd;
Thread::Thread()
    : _pthID(0),
      _isRunning(false) { cout << "Thread()" << endl; }

void Thread::startThread()
{
    // this 当前的子线程来作为threadFunc的参数
    cout << "startThread... " << endl;
    pthread_create(&_pthID, NULL, threadFunc, this);
    _isRunning = true;
}

void *Thread::threadFunc(void *arg)
{
    Thread *pThread = static_cast<Thread *>(arg);
    if (pThread)
    {
        // 子线程执行 run作为虚函数，由子线程类重写
        pThread->run();
    }
}

void Thread::joinThread()
{
    cout << "joinThread... " << endl;
    pthread_join(_pthID, NULL); // 阻塞，等待线程执行完成后才会回收
    cout << "joinThread done... " << endl;
    _isRunning = false;
}

Thread::~Thread()
{
    if (_isRunning)
    {
        cout << "~Thread() pthread_detach... " << endl;
        pthread_detach(_pthID); // 非阻塞， 直接回收当前_pthID线程资源
        _isRunning = false;
    }
}