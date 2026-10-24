#ifndef CRANK_TALK_THREADS_H
#define CRANK_TALK_THREADS_H

#include "common.h"

#ifdef _WIN32
#error Implement the win32 threading primitives
#else
#include <pthread.h>

typedef pthread_mutex_t crank_talk_Mutex_t;

CRANK_TALK_INLINE void
crank_talk_Mutex_init(crank_talk_Mutex_t *mutex)
{
    pthread_mutex_init(mutex, NULL);
}

CRANK_TALK_INLINE void
crank_talk_Mutex_destroy(crank_talk_Mutex_t *mutex)
{
    pthread_mutex_destroy(mutex);
}

CRANK_TALK_INLINE void
crank_talk_Mutex_lock(crank_talk_Mutex_t *mutex)
{
    pthread_mutex_lock(mutex);
}

CRANK_TALK_INLINE void
crank_talk_Mutex_unlock(crank_talk_Mutex_t *mutex)
{
    pthread_mutex_unlock(mutex);
}

#endif

#endif //CRANK_TALK_THREADS_H