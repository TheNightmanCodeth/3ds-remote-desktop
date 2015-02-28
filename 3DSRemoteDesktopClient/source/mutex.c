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
#include "mutex.h"
#include <malloc.h>

mutex* mutex_request(bool bStartLocked)
{
    mutex* pMutex = (mutex*)malloc(sizeof(mutex));
    mutex_create(pMutex, bStartLocked);
    return pMutex;
}

void mutex_create(mutex* pMutex, bool bStartLocked)
{
    svcCreateMutex(&pMutex->MutexHandle, bStartLocked);
}

void mutex_lock(mutex* pMutex)
{
    mutex_lock_time(pMutex, U64_MAX);
}

void mutex_lock_time(mutex* pMutex, u64 nanoseconds)
{
    svcWaitSynchronization(pMutex->MutexHandle, nanoseconds);
}

void mutex_unlock(mutex* pMutex)
{
    svcReleaseMutex(pMutex->MutexHandle);
}

void mutex_destroy(mutex* pMutex)
{
    svcReleaseMutex(pMutex->MutexHandle);
    svcCloseHandle(pMutex->MutexHandle);
}

void mutex_return(mutex* pMutex)
{
    mutex_destroy(pMutex);
    free(pMutex);
}
