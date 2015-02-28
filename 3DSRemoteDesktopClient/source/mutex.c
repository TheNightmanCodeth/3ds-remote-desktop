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
