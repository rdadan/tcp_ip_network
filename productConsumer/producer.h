#ifndef __PRODUCER_H__
#define __PRODUCER_H__
#include "thread.h"
#include "taskQueue.h"
namespace dd
{

    class Producer : public Thread
    {
    public:
        Producer(TaskQueue &taskQue);
        ~Producer();
        virtual void run();

    private:
        TaskQueue &_taskQue;
    };
} //
#endif