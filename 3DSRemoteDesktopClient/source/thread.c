#include "thread.h"
#include <malloc.h>
/*
typedef struct
{
    Handle ThreadHandle;
    u64* pThreadStack;
    u32 ThreadID;
    s32 ThreadStatus;
} ThreadData;
*/

void RunThread(u32 arg)
{
    ThreadData* pThread = (ThreadData*)arg;
    svcGetThreadId(&pThread->ThreadID, pThread->ThreadHandle);
    pThread->ThreadFunction((u32)pThread->pThreadArg);
	svcExitThread();
}

ThreadData* OpenThread(ThreadFunc func, void* arg)
{
    return OpenThreadP(func, arg, 0x18);
}

static int nCurCPUID = 0;
ThreadData* OpenThreadP(ThreadFunc func, void* arg, s32 priority)
{
    ThreadData* pThread = (ThreadData*)malloc(sizeof(ThreadData));
    pThread->pThreadStack = (u32*)malloc(sizeof(u32) * (THREAD_STACKSIZE));
    pThread->ThreadFunction = func;
    pThread->pThreadArg = arg;
    pThread->ThreadStatus = svcCreateThread(&pThread->ThreadHandle, RunThread, (u32)pThread, (u32*)&pThread->pThreadStack[THREAD_STACKSIZE], priority, 1);
    return pThread;
}

void CloseThread(ThreadData* pThread)
{
    if(pThread->ThreadStatus == 0)
    {
        s32 syncStatus = svcWaitSynchronization(&pThread->ThreadHandle, U64_MAX);
        svcCloseHandle(pThread->ThreadHandle);
    }

    if(pThread != NULL)
    {
        if(pThread->pThreadStack != NULL)
        {
            free(pThread->pThreadStack);
        }
        free(pThread);
    }
}
