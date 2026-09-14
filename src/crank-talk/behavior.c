#include "crank-talk/behavior.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/dictionary.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include <stdlib.h>

CRANK_TALK_API void
crank_talk_Behavior_computeLayout(crank_talk_VMContext_t *context, crank_talk_oop_t behaviorValue)
{
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t*)behaviorValue;
    assert(!crank_talk_oop_isNil(behavior->slots));
    if(!crank_talk_oop_isNil(behavior->allSlots))
        return;

    // Add the super type slots.
    crank_talk_oop_t allSlots = behavior->slots;
    crank_talk_oop_t superBehaviorValue = (crank_talk_oop_t)behavior->superclass;
    if(!crank_talk_oop_isNil(superBehaviorValue)) {
        crank_talk_Behavior_t *superBehavior = (crank_talk_Behavior_t*)superBehaviorValue;
        crank_talk_Behavior_computeLayout(context,superBehaviorValue);
        allSlots = crank_talk_Array_concat(context, superBehavior->allSlots, behavior->slots);
    }

    behavior->allSlots = allSlots;
    size_t allSlotCount = crank_talk_object_getSizeInSlots(allSlots);
    behavior->instanceSize = crank_talk_oop_encodeSmallInteger(sizeof(crank_talk_ObjectHeader_t) + allSlotCount*sizeof(crank_talk_oop_t));
    crank_talk_Behavior_enumerateSlots(context, behaviorValue);
}

CRANK_TALK_API void
crank_talk_Behavior_enumerateSlots(crank_talk_VMContext_t *context, crank_talk_oop_t behaviorValue)
{
    (void)context;
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t*)behaviorValue;
    assert(!crank_talk_oop_isNil(behavior->allSlots));

    size_t slotCount = crank_talk_object_getSizeInSlots(behavior->allSlots);
    crank_talk_oop_t *slots = crank_talk_oop_getSlotsData(behavior->allSlots);
    for(size_t i = 0; i < slotCount; ++i)
    {
        crank_talk_Slot_t *slot = (crank_talk_Slot_t *)slots[i];
        slot->index = crank_talk_oop_encodeSmallInteger(i);
    }
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Behavior_lookupSelector(crank_talk_oop_t behaviorValue, crank_talk_oop_t selector)
{
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t*)behaviorValue;
    while(behavior)
    {
        if(behavior->methodDictionary)
        {
            crank_talk_oop_t foundMethod = crank_talk_MethodDictionary_atOrNil((crank_talk_oop_t)behavior->methodDictionary, selector);
            if(!crank_talk_oop_isNil(foundMethod))
                return foundMethod;
        }

        behavior = behavior->superclass;
    }

    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_Behavior_basicNew_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t *)arguments[0];
    size_t instanceSize = crank_talk_oop_decodeSmallInteger(behavior->instanceSize);
    crank_talk_GCObjectDataType_t instanceDataType = crank_talk_oop_decodeSmallInteger(behavior->instanceDataType);
    return crank_talk_heap_allocateOopWithSizeAndType(&context->heap, instanceSize, instanceDataType, (crank_talk_oop_t)behavior);
}

static crank_talk_oop_t
crank_talk_Behavior_basicNewWith_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t *)arguments[0];
    if(!crank_talk_oop_isSmallInteger(arguments[1]))
        crank_talk_error("Expected a small integer size.");
    
    size_t variableSize = crank_talk_oop_decodeSmallInteger(arguments[1]);
    size_t instanceSize = crank_talk_oop_decodeSmallInteger(behavior->instanceSize);
    crank_talk_GCObjectDataType_t instanceDataType = crank_talk_oop_decodeSmallInteger(behavior->instanceDataType);
    size_t variableByteSize = (instanceDataType == CrankTalkGCObjectDataTypeBytes) ? variableSize : variableSize*sizeof(crank_talk_oop_t);
    return crank_talk_heap_allocateOopWithSizeAndType(&context->heap, instanceSize + variableByteSize, instanceDataType, (crank_talk_oop_t)behavior);
}

static crank_talk_oop_t
crank_talk_Behavior_new_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t instance = crank_talk_send0(context, context->roots.basicNewSymbol, self);
    return crank_talk_send0(context, context->roots.initializeSymbol, instance);
}

static crank_talk_oop_t
crank_talk_Behavior_newWith_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t variableSize = arguments[1];
    crank_talk_oop_t instance = crank_talk_send1(context, context->roots.basicNewWithSymbol, self, variableSize);
    return crank_talk_send0(context, context->roots.initializeSymbol, instance);
}

static crank_talk_oop_t
crank_talk_Behavior_environment_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    return context->roots.systemEnvironment;
}

static crank_talk_oop_t
crank_talk_Class_environment_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_Class_t *self = (crank_talk_Class_t *)arguments[0];
    return self->environment;
}

static crank_talk_oop_t
crank_talk_Metaclass_environment_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_Metaclass_t *self = (crank_talk_Metaclass_t *)arguments[0];
    if(self->thisClass)
        return self->thisClass->environment;
    return context->roots.systemEnvironment;
}

void
crank_talk_registerBehaviorPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "basicNew", 1, crank_talk_Behavior_basicNew_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "basicNew:", 2, crank_talk_Behavior_basicNewWith_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "new", 1, crank_talk_Behavior_new_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "new:", 2, crank_talk_Behavior_newWith_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "environment", 1, crank_talk_Behavior_environment_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ClassClass, "environment", 1, crank_talk_Class_environment_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.MetaclassClass, "environment", 1, crank_talk_Metaclass_environment_primitive);
}

