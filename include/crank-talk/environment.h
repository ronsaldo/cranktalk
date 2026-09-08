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

CRANK_TALK_API crank_talk_oop_t crank_talk_LexicalEnvironment_create(crank_talk_VMContext_t *context, crank_talk_oop_t parentEnvironment);
CRANK_TALK_API void crank_talk_LexicalEnvironment_setSymbolBinding(crank_talk_VMContext_t *context, crank_talk_oop_t environment, crank_talk_oop_t symbol, crank_talk_oop_t binding);
CRANK_TALK_API void crank_talk_LexicalEnvironment_setNewSymbolBinding(crank_talk_VMContext_t *context, crank_talk_oop_t environment, crank_talk_oop_t symbol, crank_talk_oop_t binding, crank_talk_oop_t sourcePosition);


#endif //CRANK_TALK_ENVIRONMENT_H
