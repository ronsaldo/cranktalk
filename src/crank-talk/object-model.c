#include "crank-talk/object-model.h"
#include "crank-talk/context.h"
#include <stdlib.h>

CRANK_TALK_API crank_talk_Behavior_t *
crank_talk_oop_getClass(crank_talk_VMContext_t *context, crank_talk_oop_t value)
{
    if(value == crank_talk_oop_nil())
        return (crank_talk_Behavior_t*)context->roots.UndefinedObjectClass;
    
    intptr_t tag = value & CRANK_TALK_OOP_TAG_MASK;
    switch(tag)
    {
    case CRANK_TALK_OOP_TAG_SMALL_INTEGER_VALUE: return (crank_talk_Behavior_t*)context->roots.SmallIntegerClass;
    case CRANK_TALK_OOP_TAG_CHARACTER_VALUE:     return (crank_talk_Behavior_t*)context->roots.CharacterClass;
    case CRANK_TALK_OOP_TAG_SMALL_FLOAT_VALUE:   return (crank_talk_Behavior_t*)context->roots.SmallFloatClass;
    case 0:                                      return crank_talk_object_getBehavior(value);
    default: abort();
    }
}