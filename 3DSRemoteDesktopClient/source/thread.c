/*
Copyright (c) 2015, Erick<RetroZelda>Folckemer
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

void thread_run_func(u32 arg)
{
    ThreadData* pThread = (ThreadData*)arg;
    svcGetThreadId(&pThread->ThreadID, pThread->ThreadHandle);
    pThread->ThreadFunction((u32)pThread->pThreadArg);
    pThread->ThreadStatus = THREAD_NOTRUNNING;
	svcExitThread();
}

ThreadData* thread_request(ThreadFunc func, void* arg)
{
    return thread_requestP(func, arg, 0x18);
}

ThreadData* thread_requestP(ThreadFunc func, void* arg, s32 priority)
{
    ThreadData* pThread = (ThreadData*)malloc(sizeof(ThreadData));
    pThread->pThreadStack = (u32*)malloc(sizeof(u32) * (THREAD_STACKSIZE));
    pThread->ThreadFunction = func;
    pThread->pThreadArg = arg;
    pThread->ThreadPriority = priority;
    pThread->ThreadStatus = THREAD_NOTRUNNING;
    return pThread;
}


void thread_start(ThreadData* pThread)
{
    if(pThread->ThreadStatus != THREAD_RUNNING)
    {
        pThread->ThreadStatus = svcCreateThread(&pThread->ThreadHandle, thread_run_func, (u32)pThread,
                                             (u32*)&pThread->pThreadStack[THREAD_STACKSIZE], pThread->ThreadPriority, 1);
    }
}

void thread_startF(ThreadData* pThread, ThreadFunc func, void* arg)
{
    pThread->ThreadFunction = func;
    pThread->pThreadArg = arg;
    thread_start(pThread);
}

void thread_startP(ThreadData* pThread, ThreadFunc func, void* arg, s32 priority)
{
    pThread->ThreadPriority = priority;
    thread_startF(pThread, func, arg);
}

void thread_close(ThreadData* pThread)
{
    svcCloseHandle(pThread->ThreadHandle);

    if(pThread != NULL)
    {
        if(pThread->pThreadStack != NULL)
        {
            free(pThread->pThreadStack);
        }
        free(pThread);
    }
}
