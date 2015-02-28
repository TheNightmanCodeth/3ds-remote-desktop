#ifndef _THREAD_H_
#define _THREAD_H_

#include <3ds.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_RUNNING 0
#define THREAD_STACKSIZE 0x1000 >> 3

typedef struct
{
    Handle ThreadHandle;
    ThreadFunc ThreadFunction;
    u64* pThreadStack;
    void* pThreadArg;
    u32 ThreadID;
    s32 ThreadStatus;
} ThreadData;


ThreadData* OpenThread(ThreadFunc func, void* arg);
ThreadData* OpenThreadP(ThreadFunc func, void* arg, s32 priority);

void CloseThread(ThreadData* pThread);

#ifdef __cplusplus
}
#endif

#endif // _THREAD_H_
