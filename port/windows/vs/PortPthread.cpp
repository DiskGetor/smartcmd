#include "utility/misc/ThreadUtil.h"
#include "PortPthread.h"

DWORD WINAPI ThreadFuncWrapper(LPVOID param)
{
    if(NULL == param) return 1;

    sPthreadInfo* info = (sPthreadInfo*)param;

    info->func(info->arg);

    delete param;

    return 0;
}

void InitPthreadType(sPthreadType* obj)
{
    if(NULL == obj) return;
    obj->handle = INVALID_HANDLE_VALUE;
}

void InitPthreadCond(sPthreadCond* obj)
{
    if(NULL == obj) return;
    InitializeConditionVariable(&obj->variable);
}

void InitPthreadMutex(sPthreadMutex* obj)
{
    if(NULL == obj) return;
    obj->handle = CreateMutex(NULL, FALSE, NULL);
}

int PthreadMutexLock(sPthreadMutex* obj)
{
    if(NULL == obj) return -1;
    if(WAIT_FAILED == WaitForSingleObject(obj->handle, INFINITE)) return -1;
    return 0;
}

int PthreadMutexUnlock(sPthreadMutex* obj)
{
    if(NULL == obj) return -1;
    if(false == ReleaseMutex(obj->handle)) return -1;
    return 0;
}

int PthreadJoin(sPthreadType obj)
{
    if(WAIT_FAILED == WaitForSingleObject(obj.handle, INFINITE)) return -1;
    return 0;
}

int PthreadCondSignal(sPthreadCond* obj)
{
    if(NULL == obj) return -1;
    WakeConditionVariable(&obj->variable);
    return 0;
}

int PthreadCondTimedwait(sPthreadCond* cond, sPthreadMutex* mutex, int timeoutInSec)
{
    if(NULL == cond) return -1;
    if(NULL == mutex) return -1;

    bool retVal;
    CRITICAL_SECTION criticalSection;

    InitializeCriticalSection(&criticalSection);
    EnterCriticalSection(&criticalSection);

    PthreadMutexUnlock(mutex);

    retVal = SleepConditionVariableCS(&cond->variable, &criticalSection, timeoutInSec*1000);

    LeaveCriticalSection(&criticalSection);

    PthreadMutexLock(mutex);

    return ((true == retVal) ? 0 : -1);
}

int PthreadCreate(sPthreadType* thread, tPthreadFunc func, void* arg)
{
    if(NULL == thread) return -1;
    if(NULL == func) return -1;

    sPthreadInfo* param = new sPthreadInfo;
    param->func = func;
    param->arg = (LPVOID) arg;

    thread->handle = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFuncWrapper, param, 0, NULL );
    
    if(INVALID_HANDLE_VALUE == thread->handle) return -1;

    return 0;
}
