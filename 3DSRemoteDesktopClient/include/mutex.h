#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <3ds.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    volatile Handle MutexHandle;
} mutex;

mutex* mutex_request(bool bStartLocked);
void mutex_create(mutex* pMutex, bool bStartLocked);
void mutex_lock(mutex* pMutex);
void mutex_lock_time(mutex* pMutex, u64 nanoseconds);
void mutex_unlock(mutex* pMutex);
void mutex_destroy(mutex* pMutex);
void mutex_return(mutex* pMutex);

#ifdef __cplusplus
}
#endif

#endif // _MUTEX_H_
