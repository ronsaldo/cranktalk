#ifndef CRANK_TALK_RANDOM_H
#define CRANK_TALK_RANDOM_H

#include "object-model.h"

typedef struct crank_talk_Random_s crank_talk_Random_t;

struct crank_talk_Random_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t seed;
};

#endif //CRANK_TALK_RANDOM_H
