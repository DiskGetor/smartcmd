#ifndef PORTPTHREAD_H
#define PORTPTHREAD_H

#include <pthread.h>

struct sPthreadType
{
    pthread_t handle;
};

struct sPthreadMutex
{
    pthread_mutex_t handle;
};

struct sPthreadCond
{
    pthread_cond_t handle;
};

#endif
