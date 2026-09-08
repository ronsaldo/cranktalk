#ifndef CRANK_TALK_ENVIRONMENT_H
#define CRANK_TALK_ENVIRONMENT_H

#include "object-model.h"

typedef struct crank_talk_AbstractEnvironment_s crank_talk_AbstractEnvironment_t;
typedef struct crank_talk_EmptyEnvironment_s crank_talk_EmptyEnvironment_t;
typedef struct crank_talk_SystemEnvironmentEnvironment_s crank_talk_SystemEnvironmentEnvironment_t;
typedef struct crank_talk_LexicalEnvironment_s crank_talk_LexicalEnvironment_t;

struct crank_talk_AbstractEnvironment_s
{
    crank_talk_Object_t super;
};

struct crank_talk_EmptyEnvironment_s
{
    crank_talk_AbstractEnvironment_t super;
};

struct crank_talk_SystemEnvironmentEnvironment_s
{
    crank_talk_AbstractEnvironment_t super;
    crank_talk_oop_t parent;
    crank_talk_oop_t systemEnvironment;
};

struct crank_talk_LexicalEnvironment_s
{
    crank_talk_AbstractEnvironment_t super;
    crank_talk_oop_t parent;
    crank_talk_oop_t symbolTable;
};

#endif //CRANK_TALK_ENVIRONMENT_H
