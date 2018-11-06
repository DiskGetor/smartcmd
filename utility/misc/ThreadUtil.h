#ifndef THREADUTIL_H
#define THREADUTIL_H

typedef void* (*tPthreadFunc)(void *);

#ifdef __LINUX__
#include "port/linuxtest/PortPthread.h"
#endif

#ifdef __MVISTA__
#include "port/linuxtest/PortPthread.h"
#endif

#ifdef __FREEBSD__
#include "port/linuxtest/PortPthread.h"
#endif

#ifdef __WIN32QT__
#include "port/windows/qt/PortPthread.h"
#endif

#ifdef __WIN32VS__
#include "port/windows/vs/PortPthread.h"
#endif

void InitPthreadType(sPthreadType* obj);
void InitPthreadCond(sPthreadCond* obj);
void InitPthreadMutex(sPthreadMutex* obj);

// Common pthread functions
int PthreadMutexLock(sPthreadMutex* obj);
int PthreadMutexUnlock(sPthreadMutex* obj);

int PthreadCreate(sPthreadType* thread, tPthreadFunc func, void* arg);
int PthreadJoin(sPthreadType obj);

int PthreadCondSignal(sPthreadCond* obj);
int PthreadCondTimedwait(sPthreadCond* cond, sPthreadMutex* mutex, int timeoutInSec);

#endif
