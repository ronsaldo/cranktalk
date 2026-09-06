#include "crank-talk/object-model.h"
#include "crank-talk/context.h"
#include <stdlib.h>

CRANK_TALK_API double
crank_talk_oop_numberAsFloat(crank_talk_oop_t oop)
{
    if(crank_talk_oop_isSmallFloat(oop))
        return crank_talk_oop_decodeSmallFloat(oop);
    else if(crank_talk_oop_isSmallInteger(oop))
        return crank_talk_oop_decodeSmallInteger(oop);
    return ((crank_talk_BoxedFloat_t*)oop)->value;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_oop_fromBoolean(crank_talk_VMContext_t *context, bool value)
{
    return value ? context->roots.trueValue : context->roots.falseValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_oop_fromFloat(crank_talk_VMContext_t *context, double value)
{
    if(crank_talk_oop_isFloatInSmallFloatRange(value))
    {
        return crank_talk_oop_encodeSmallFloat(value);
    }
    else
    {
        crank_talk_oop_t boxedFloatValue = CrankAllocateBytesObjectWithClassName(context, BoxedFloat);
        crank_talk_BoxedFloat_t *boxedFloat = (crank_talk_BoxedFloat_t *)boxedFloatValue;
        boxedFloat->value = value;
        return boxedFloatValue;
    }
}

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

CRANK_TALK_API bool
sysmel_vm_Behavior_isDirectSubtype(crank_talk_oop_t type, crank_talk_oop_t supertype)
{
    if(crank_talk_oop_isNil(type) || crank_talk_oop_isNil(supertype))
        return false;

    crank_talk_oop_t position = type;
    while(!crank_talk_oop_isNil(position))
    {
        if(position == supertype)
            return true;

        crank_talk_Behavior_t *positionBehavior = (crank_talk_Behavior_t*)position;
        position = (crank_talk_oop_t)positionBehavior->superclass;
    }

    return false;
}

CRANK_TALK_API bool
crank_talk_oop_isKindOf(crank_talk_VMContext_t *context, crank_talk_oop_t value, crank_talk_oop_t behavior)
{
    crank_talk_oop_t valueClass = (crank_talk_oop_t)crank_talk_oop_getClass(context, value);
    return value == behavior || sysmel_vm_Behavior_isDirectSubtype(valueClass, behavior);
}
