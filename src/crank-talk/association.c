#include "crank-talk/association.h"
#include "crank-talk/context.h"

CRANK_TALK_API crank_talk_oop_t
crank_talk_Association_create(crank_talk_VMContext_t *context, crank_talk_oop_t key, crank_talk_oop_t value)
{
    crank_talk_oop_t associationValue = CrankAllocateValuesObjectWithClassName(context, Association);
    crank_talk_Association_t *assoc = (crank_talk_Association_t *)associationValue;
    assoc->super.key = key;
    assoc->value = value;
    return associationValue;
}
