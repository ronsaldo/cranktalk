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

CRANK_TALK_INLINE float crank_talk_fclamp(float x, float minValue, float maxValue)
{
    return crank_talk_fmin(crank_talk_fmax(x, minValue), maxValue);
}

#endif //CRANK_TALK_UTILS_H
