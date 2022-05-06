#include "condition.h"
#include "mutex.h"
using namespace dd;

Condition::Condition(Mutex &mutex)
    : _condMutex(mutex)
{
    pthread_cond_init(&_cond, NULL);
}
Condition::~Condition()
{
    pthread_cond_destroy(&_cond);
}

void Condition::condWait()
{
    pthread_cond_wait(&_cond, _condMutex.getMutexPtr());
}
void Condition::condNotify()
{
    pthread_cond_signal(&_cond);
}
void Condition::condNotifyALL()
{
    pthread_cond_broadcast(&_cond);
}
