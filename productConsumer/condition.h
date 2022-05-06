#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "nonCopyable.h"
#include <pthread.h>

namespace dd
{
    class Mutex; //前向声明减少头文件依赖

    // condition 和 mutex 搭配使用，
    // 建立单向关联关系， condition use mutex
    //
    class Condition : public nonCopyable
    {
    public:
        Condition(Mutex &mutex);
        ~Condition();

        void condWait();
        void condNotify();
        void condNotifyALL();

    private:
        pthread_cond_t _cond;
        Mutex &_condMutex;
    };

} // end space
#endif