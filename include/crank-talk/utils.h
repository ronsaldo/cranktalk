#ifndef CRANK_TALK_UTILS_H
#define CRANK_TALK_UTILS_H

#include "common.h"

CRANK_TALK_INLINE float crank_talk_fmin(float a, float b)
{
    return a < b ? a : b;
}

CRANK_TALK_INLINE float crank_talk_fmax(float a, float b)
{
    return a < b ? b : a;
}
#endif //CRANK_TALK_UTILS_H
