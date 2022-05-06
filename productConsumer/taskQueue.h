#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__
#include "mutex.h"
#include "condition.h"
#include <cstddef>
#include <queue>
using std::queue;

// 任务队列是临界资源，线程共享
// Mutex,Condition类和 TaskQUEUE建立组合关系
namespace dd
{
    using Product = int;
    class TaskQueue
    {

    public:
        TaskQueue(size_t queSize);
        size_t getQueSize();
        void pushProduct(Product product);
        Product popProduct();

        bool isEmpty();
        bool isFull();

    private:
        size_t _queSize;
        Mutex _mutex;
        Condition _condNotEmpty;
        Condition _condNotFull;
        queue<Product> _que;
    };

}
#endif