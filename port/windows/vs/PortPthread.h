#ifndef PORTPTHREAD_H
#define PORTPTHREAD_H

#include <windows.h>

struct sPthreadType
{
    HANDLE handle;
};

struct sPthreadMutex
{
    HANDLE handle;
};

struct sPthreadCond
{
    CONDITION_VARIABLE variable;
};

struct sPthreadInfo
{
    LPVOID arg;
    tPthreadFunc func;
};

DWORD WINAPI ThreadFuncWrapper(LPVOID param);

#endif