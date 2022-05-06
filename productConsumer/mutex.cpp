#include "mutex.h"
using namespace dd;

Mutex::Mutex()
    : _isLocking(false)
{
    pthread_mutex_init(&_mutex, NULL);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&_mutex);
}

void Mutex::mutexLock()
{
    pthread_mutex_lock(&_mutex);
    _isLocking = true;
}
void Mutex::mutexUnlock()
{
    pthread_mutex_unlock(&_mutex);
    _isLocking = false;
}
pthread_mutex_t *Mutex::getMutexPtr()
{
    return &_mutex;
}
