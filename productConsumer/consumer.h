#ifndef __CONSUMER_H__
#define __CONSUMER_H__
#include "thread.h"
#include "taskQueue.h"
namespace dd
{

    class Consumer : public Thread
    {
    public:
        Consumer(TaskQueue &taskQue);
        ~Consumer();
        virtual void run();

    private:
        TaskQueue &_taskQue;
    };
} //
#endif