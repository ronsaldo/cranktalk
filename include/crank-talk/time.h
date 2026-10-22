#ifndef CRANK_TALK_TIME_H
#define CRANK_TALK_TIME_H

#include "object-model.h"

typedef struct crank_talk_Time_s crank_talk_Time_t;

struct crank_talk_Time_s
{
    crank_talk_Magnitude_t super;
    crank_talk_oop_t seconds;
    crank_talk_oop_t nanos;
};

CRANK_TALK_API int64_t crank_talk_getMicrosecondClockValue(void);

#endif // CRANK_TALK_TIME_H