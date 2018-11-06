#include <time.h>
#include "utility/misc/ThreadUtil.h"
#include "PortPthread.h"

void InitPthreadType(sPthreadType* obj)
{
    if(NULL == obj) return;
    obj->handle = 0;
}

void InitPthreadCond(sPthreadCond* obj)
{
    if(NULL == obj) return;
    obj->handle = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
}

void InitPthreadMutex(sPthreadMutex* obj)
{
    if(NULL == obj) return;
    obj->handle = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
}

int PthreadMutexLock(sPthreadMutex* obj)
{
    if(NULL == obj) return -1;
    return pthread_mutex_lock(&obj->handle);
}

int PthreadMutexUnlock(sPthreadMutex* obj)
{
    if(NULL == obj) return -1;
    return pthread_mutex_unlock(&obj->handle);
}

int PthreadJoin(sPthreadType obj)
{
    return pthread_join(obj.handle, NULL);
}

int PthreadCondSignal(sPthreadCond* obj)
{
    if(NULL == obj) return -1;
    return pthread_cond_signal(&obj->handle);
}

int PthreadCondTimedwait(sPthreadCond* cond, sPthreadMutex* mutex, int timeoutInSec)
{
    if(NULL == cond) return -1;
    if(NULL == mutex) return -1;

    unsigned int timeval = time(0);

    struct timespec timeout;
    timeout.tv_sec = timeval + timeoutInSec;
    timeout.tv_nsec = 0;
    
    return pthread_cond_timedwait(&cond->handle, &mutex->handle, &timeout);
}

int PthreadCreate(sPthreadType* thread, tPthreadFunc func, void* arg)
{
    if(NULL == thread) return -1;
    if(NULL == func) return -1;

    return pthread_create(&thread->handle, NULL, func, arg);
}
