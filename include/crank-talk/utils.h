#ifndef CRANK_TALK_UTILS_H
#define CRANK_TALK_UTILS_H

#include "common.h"
#include <math.h>

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

CRANK_TALK_INLINE float crank_talk_fmix(float x, float y, float a)
{
    return x *(1 - a) + y*a;
}

CRANK_TALK_INLINE float crank_talk_floorMod(float a, float d)
{
    return a - floor(a/d)*d;
}

#endif //CRANK_TALK_UTILS_H
