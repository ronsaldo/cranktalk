#include "crank-talk/behavior.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/dictionary.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include <stdlib.h>
#include <stdio.h>

CRANK_TALK_API crank_talk_oop_t
crank_talk_Behavior_createAnonymousClassAndMetaclass(crank_talk_VMContext_t *context, crank_talk_oop_t superclass, size_t instanceSize, crank_talk_GCObjectDataType_t dataType)
{
    crank_talk_oop_t metaclassValue =  CrankAllocateValuesObjectWithClassName(context, Metaclass);
    crank_talk_Metaclass_t *metaclass = (crank_talk_Metaclass_t*)metaclassValue;
    if(!crank_talk_oop_isNil(superclass))
        metaclass->super.super.superclass = crank_talk_oop_getClass(context, superclass);

    metaclass->super.super.instanceSize      = crank_talk_oop_encodeSmallInteger(sizeof(crank_talk_Class_t));
    metaclass->super.super.instanceDataType  = crank_talk_oop_encodeSmallInteger(CrankTalkGCObjectDataTypeOops);

    crank_talk_oop_t classValue = crank_talk_heap_allocateOopWithSizeAndType(&context->heap, sizeof(crank_talk_Class_t), CrankTalkGCObjectDataTypeOops, metaclassValue);
    crank_talk_Class_t *clazz = (crank_talk_Class_t*)classValue;
    clazz->super.super.superclass = (crank_talk_Behavior_t*)superclass;
    clazz->super.super.instanceSize      = crank_talk_oop_encodeSmallInteger(instanceSize);
    clazz->super.super.instanceDataType  = crank_talk_oop_encodeSmallInteger(dataType);
    metaclass->thisClass = clazz;
    return classValue;
}

CRANK_TALK_API void
crank_talk_Behavior_createBehaviorSlotsTables(crank_talk_VMContext_t *context, crank_talk_oop_t behaviorValue)
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
        crank_talk_Behavior_createBehaviorSlotsTables(context,superBehaviorValue);
        if(!crank_talk_oop_isNil(behavior->allSlots))
            return;

        allSlots = crank_talk_Array_concat(context, superBehavior->allSlots, behavior->slots);
    }
    behavior->allSlots = allSlots;

    crank_talk_Behavior_enumerateSlots(context, behaviorValue);
}

CRANK_TALK_API void
crank_talk_Behavior_createClassTables(crank_talk_VMContext_t *context, crank_talk_oop_t classValue)
{
    crank_talk_Behavior_createBehaviorSlotsTables(context, classValue);
    crank_talk_oop_t metaclass = (crank_talk_oop_t)crank_talk_oop_getClass(context, classValue);
    crank_talk_Behavior_createBehaviorSlotsTables(context, metaclass);
}

CRANK_TALK_API void
crank_talk_Behavior_createClassTableAndComputeLayouts(crank_talk_VMContext_t *context, crank_talk_oop_t classValue)
{
    crank_talk_Behavior_computeLayout(context, classValue);
    crank_talk_oop_t metaclass = (crank_talk_oop_t)crank_talk_oop_getClass(context, classValue);
    crank_talk_Behavior_computeLayout(context, metaclass);
}

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
        slot->index = crank_talk_oop_encodeSmallInteger(i + 1);
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

CRANK_TALK_API crank_talk_oop_t
crank_talk_Behavior_lookupSlotNamed(crank_talk_oop_t behaviorValue, crank_talk_oop_t name)
{
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t*)behaviorValue;
    assert(behavior->allSlots);
    size_t slotCount = crank_talk_object_getSizeInSlots(behavior->allSlots);
    crank_talk_oop_t *slots = crank_talk_oop_getSlotsData(behavior->allSlots);
    for(size_t i = 0; i < slotCount; ++i)
    {
        crank_talk_Slot_t *slot = (crank_talk_Slot_t *)slots[i];
        if(slot->super.name == (crank_talk_Symbol_t*)name)
            return (crank_talk_oop_t)slot;
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
        crank_talk_error(context, "Expected a small integer size.");
    
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
    (void)arguments;
    assert(argumentCount > 0);
    return context->roots.systemEnvironment;
}

static crank_talk_oop_t
crank_talk_Class_environment_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
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

static crank_talk_oop_t
crank_talk_Behavior_makeClassBuilder_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t subclassName = arguments[1];
    if(!crank_talk_oop_isKindOf(context, subclassName, context->roots.SymbolClass))
        crank_talk_error(context, "Expected a symbol for the class name.");

    crank_talk_oop_t environment = crank_talk_send0(context, context->roots.environmentSymbol, self);

    crank_talk_oop_t classBuilderValue = CrankAllocateValuesObjectWithClassName(context, ShiftClassBuilder);
    crank_talk_ShiftClassBuilder_t *classBuilder = (crank_talk_ShiftClassBuilder_t *)classBuilderValue;
    classBuilder->superclass = self;
    classBuilder->name = (crank_talk_Symbol_t*)subclassName;
    classBuilder->environment = environment;
    classBuilder->slots = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    return classBuilderValue;
}

static crank_talk_oop_t
crank_talk_ShiftClassBuilder_slots_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t slotDefinitions = arguments[1];

    crank_talk_ShiftClassBuilder_t *classBuilder = (crank_talk_ShiftClassBuilder_t *)self;
    if(!crank_talk_oop_isKindOf(context, slotDefinitions, context->roots.ArrayClass))
        crank_talk_error(context, "Expected an array with the slots.");

    size_t slotCount = crank_talk_object_getSizeInSlots(slotDefinitions);
    crank_talk_oop_t *slotDefinitionValues = crank_talk_oop_getSlotsData(slotDefinitions);
    crank_talk_Array_t *slots = (crank_talk_Array_t*)crank_talk_Array_create(context, slotCount);
    classBuilder->slots = slots;

    for(size_t i = 0; i < slotCount; ++i)
    {
        crank_talk_oop_t slotDefinition = slotDefinitionValues[i];
        crank_talk_oop_t slotValue = crank_talk_send0(context, context->roots.asSlotSymbol, slotDefinition);
        slots->elements[i] = slotValue;
    }

    return self;
}

static crank_talk_oop_t
crank_talk_ShiftClassBuilder_tag_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    return self;
}

static crank_talk_oop_t
crank_talk_ShiftClassBuilder_package_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    return self;
}

static crank_talk_oop_t
crank_talk_ShiftClassBuilder_build_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t self = arguments[0];

    crank_talk_ShiftClassBuilder_t *classBuilder = (crank_talk_ShiftClassBuilder_t *)self;
    crank_talk_oop_t classValue = crank_talk_Behavior_createAnonymousClassAndMetaclass(context, classBuilder->superclass, 0, CrankTalkGCObjectDataTypeOops);
    crank_talk_Class_t *clazz = (crank_talk_Class_t *)classValue;
    clazz->super.super.slots = (crank_talk_oop_t)classBuilder->slots;
    clazz->name = classBuilder->name;
    clazz->environment = classBuilder->environment;

    crank_talk_oop_t metaclassValue = (crank_talk_oop_t)crank_talk_oop_getClass(context, classValue);
    assert(!crank_talk_oop_isNil(metaclassValue));
    crank_talk_Metaclass_t *metaClass = (crank_talk_Metaclass_t*)metaclassValue;
    if(crank_talk_oop_isNil(metaClass->super.super.slots))
        metaClass->super.super.slots = crank_talk_Array_create(context, 0);

    crank_talk_Behavior_createClassTableAndComputeLayouts(context, classValue);
    if(clazz->name && clazz->environment)
        crank_talk_SystemEnvironment_atPut(context, context->roots.systemEnvironment, (crank_talk_oop_t)clazz->name, classValue);

    return classValue;
}

static crank_talk_oop_t
crank_talk_Slot_asSlot_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t self = arguments[0];
    return self;
}

static crank_talk_oop_t
crank_talk_Symbol_asSlot_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t slotValue = CrankAllocateValuesObjectWithClassName(context, Slot);
    crank_talk_Slot_t *slot = (crank_talk_Slot_t *)slotValue;
    slot->super.name = (crank_talk_Symbol_t*)self;
    return slotValue;
}

void
crank_talk_registerBehaviorPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.BehaviorClass, "basicNew", 1, crank_talk_Behavior_basicNew_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.BehaviorClass, "basicNew:", 2, crank_talk_Behavior_basicNewWith_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.BehaviorClass, "new", 1, crank_talk_Behavior_new_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.BehaviorClass, "new:", 2, crank_talk_Behavior_newWith_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.BehaviorClass, "environment", 1, crank_talk_Behavior_environment_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.ClassClass, "environment", 1, crank_talk_Class_environment_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.MetaclassClass, "environment", 1, crank_talk_Metaclass_environment_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.BehaviorClass, "<<", 2, crank_talk_Behavior_makeClassBuilder_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "slots:", 2, crank_talk_ShiftClassBuilder_slots_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "tag:", 2, crank_talk_ShiftClassBuilder_tag_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "package:", 2, crank_talk_ShiftClassBuilder_package_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "build", 1, crank_talk_ShiftClassBuilder_build_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SlotClass, "asSlot", 1, crank_talk_Slot_asSlot_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SymbolClass, "asSlot", 1, crank_talk_Symbol_asSlot_primitive);
}

