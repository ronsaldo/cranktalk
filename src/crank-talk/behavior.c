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
crank_talk_Behavior_registerClassInSuperclass(crank_talk_VMContext_t *context, crank_talk_Class_t *clazz)
{
    crank_talk_Class_t *superclass = (crank_talk_Class_t *)clazz->super.super.superclass;
    if(!superclass)
        return;
    superclass->subclasses = (crank_talk_Array_t*)crank_talk_Array_copyWith(context, (crank_talk_oop_t)superclass->subclasses, (crank_talk_oop_t)clazz);
}

CRANK_TALK_API void
crank_talk_Behavior_createClassTables(crank_talk_VMContext_t *context, crank_talk_oop_t classValue)
{
    crank_talk_Behavior_registerClassInSuperclass(context, (crank_talk_Class_t*)classValue);
    crank_talk_Behavior_createBehaviorSlotsTables(context, classValue);
    crank_talk_oop_t metaclass = (crank_talk_oop_t)crank_talk_oop_getClass(context, classValue);
    crank_talk_Behavior_createBehaviorSlotsTables(context, metaclass);
}

CRANK_TALK_API void
crank_talk_Behavior_createClassTableAndComputeLayouts(crank_talk_VMContext_t *context, crank_talk_oop_t classValue)
{
    crank_talk_Behavior_registerClassInSuperclass(context, (crank_talk_Class_t*)classValue);
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

CRANK_TALK_API crank_talk_oop_t
crank_talk_Behavior_lookupClassBinding(crank_talk_VMContext_t *context, crank_talk_oop_t behavior, crank_talk_oop_t name)
{
    return crank_talk_send1(context, context->roots.lookupClassBindingSymbol, behavior, name);
}

static void
crank_talk_Behavior_basicNew_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
 
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t *)primContext->arguments[0];
    size_t instanceSize = crank_talk_oop_decodeSmallInteger(behavior->instanceSize);
    crank_talk_GCObjectDataType_t instanceDataType = crank_talk_oop_decodeSmallInteger(behavior->instanceDataType);
    crank_talk_oop_t result = crank_talk_heap_allocateOopWithSizeAndType(&primContext->context->heap, instanceSize, instanceDataType, (crank_talk_oop_t)behavior);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_Behavior_basicNewWith_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t *)primContext->arguments[0];
    if(!crank_talk_oop_isSmallInteger(primContext->arguments[1]))
        crank_talk_error(primContext->context, "Expected a small integer size.");
    
    size_t variableSize = crank_talk_oop_decodeSmallInteger(primContext->arguments[1]);
    size_t instanceSize = crank_talk_oop_decodeSmallInteger(behavior->instanceSize);
    crank_talk_GCObjectDataType_t instanceDataType = crank_talk_oop_decodeSmallInteger(behavior->instanceDataType);
    size_t variableByteSize = (instanceDataType == CrankTalkGCObjectDataTypeBytes) ? variableSize : variableSize*sizeof(crank_talk_oop_t);
    crank_talk_oop_t result = crank_talk_heap_allocateOopWithSizeAndType(&primContext->context->heap, instanceSize + variableByteSize, instanceDataType, (crank_talk_oop_t)behavior);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_Behavior_new_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t instance = crank_talk_send0(primContext->context, primContext->context->roots.basicNewSymbol, self);
    crank_talk_oop_t result = crank_talk_send0(primContext->context, primContext->context->roots.initializeSymbol, instance);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_Behavior_newWith_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t variableSize = primContext->arguments[1];
    crank_talk_oop_t instance = crank_talk_send1(primContext->context, primContext->context->roots.basicNewWithSymbol, self, variableSize);
    crank_talk_oop_t result = crank_talk_send0(primContext->context, primContext->context->roots.initializeSymbol, instance);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_Behavior_environment_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, primContext->context->roots.systemEnvironment);
}

static void
crank_talk_Class_environment_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_Class_t *self = (crank_talk_Class_t *)primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, self->environment);
}

static void
crank_talk_Metaclass_environment_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    
    crank_talk_Metaclass_t *self = (crank_talk_Metaclass_t *)primContext->arguments[0];
    if(self->thisClass)
        return crank_talk_Primitive_return(primContext, self->thisClass->environment);
    return crank_talk_Primitive_return(primContext, primContext->context->roots.systemEnvironment);
}

static void
crank_talk_Class_lookupClassBinding_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_Class_t *self = (crank_talk_Class_t *)primContext->arguments[0];
    crank_talk_oop_t name = primContext->arguments[1];

    // Check the class pool
    if(self->classPool)
    {
        crank_talk_oop_t classVariable = crank_talk_IdentityDictionary_associationAtOrNil((crank_talk_oop_t)self->classPool, name);
        if(classVariable)
            return crank_talk_Primitive_return(primContext, classVariable);
    }

    // Check the shared pools.
    if(self->sharedPools)
    {
        size_t sharedPoolCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)self->sharedPools);
        for(size_t i = 0; i < sharedPoolCount; ++i)
        {
            crank_talk_oop_t sharedPool = self->sharedPools->elements[i];
            crank_talk_oop_t binding = crank_talk_Behavior_lookupClassBinding(primContext->context, sharedPool, name);
            if(binding)
                return crank_talk_Primitive_return(primContext, binding);
        }
    }

    // Delegate to the super class
    if(self->super.super.superclass)
        return crank_talk_Primitive_return(primContext, crank_talk_Behavior_lookupClassBinding(primContext->context, (crank_talk_oop_t)self->super.super.superclass, name));

    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_Metaclass_lookupClassBinding_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_Metaclass_t *self = (crank_talk_Metaclass_t *)primContext->arguments[0];
    crank_talk_oop_t name = primContext->arguments[1];
    if (!self->thisClass)
        return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());

    return crank_talk_Primitive_return(primContext, crank_talk_Behavior_lookupClassBinding(primContext->context, (crank_talk_oop_t)self->thisClass, name));
}

static void
crank_talk_Behavior_makeClassBuilder_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t subclassName = primContext->arguments[1];
    if(!crank_talk_oop_isKindOf(primContext->context, subclassName, primContext->context->roots.SymbolClass))
        crank_talk_error(primContext->context, "Expected a symbol for the class name.");

    crank_talk_oop_t environment = crank_talk_send0(primContext->context, primContext->context->roots.environmentSymbol, self);

    crank_talk_oop_t classBuilderValue = CrankAllocateValuesObjectWithClassName(primContext->context, ShiftClassBuilder);
    crank_talk_ShiftClassBuilder_t *classBuilder = (crank_talk_ShiftClassBuilder_t *)classBuilderValue;
    classBuilder->superclass = self;
    classBuilder->name = (crank_talk_Symbol_t*)subclassName;
    classBuilder->environment = environment;
    classBuilder->slots = (crank_talk_Array_t*)crank_talk_Array_create(primContext->context, 0);
    classBuilder->sharedVariables = (crank_talk_Array_t*)crank_talk_Array_create(primContext->context, 0);
    classBuilder->sharedPools = (crank_talk_Array_t*)crank_talk_Array_create(primContext->context, 0);
    return crank_talk_Primitive_return(primContext, classBuilderValue);
}

static void
crank_talk_ShiftClassBuilder_slots_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t slotDefinitions = primContext->arguments[1];

    crank_talk_ShiftClassBuilder_t *classBuilder = (crank_talk_ShiftClassBuilder_t *)self;
    if(!crank_talk_oop_isKindOf(primContext->context, slotDefinitions, primContext->context->roots.ArrayClass))
        crank_talk_error(primContext->context, "Expected an array with the slots.");

    size_t slotCount = crank_talk_object_getSizeInSlots(slotDefinitions);
    crank_talk_oop_t *slotDefinitionValues = crank_talk_oop_getSlotsData(slotDefinitions);
    crank_talk_Array_t *slots = (crank_talk_Array_t*)crank_talk_Array_create(primContext->context, slotCount);
    classBuilder->slots = slots;

    for(size_t i = 0; i < slotCount; ++i)
    {
        crank_talk_oop_t slotDefinition = slotDefinitionValues[i];
        crank_talk_oop_t slotValue = crank_talk_send0(primContext->context, primContext->context->roots.asSlotSymbol, slotDefinition);
        slots->elements[i] = slotValue;
    }

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_ShiftClassBuilder_sharedVariables_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t sharedVariableDefinitions = primContext->arguments[1];

    crank_talk_ShiftClassBuilder_t *classBuilder = (crank_talk_ShiftClassBuilder_t *)self;
    if(!crank_talk_oop_isKindOf(primContext->context, sharedVariableDefinitions, primContext->context->roots.ArrayClass))
        crank_talk_error(primContext->context, "Expected an array with the shared variables.");

    size_t variableCount = crank_talk_object_getSizeInSlots(sharedVariableDefinitions);
    crank_talk_oop_t *sharedVariablesDefinitionValues = crank_talk_oop_getSlotsData(sharedVariableDefinitions);
    crank_talk_Array_t *sharedVariables = (crank_talk_Array_t*)crank_talk_Array_create(primContext->context, variableCount);
    classBuilder->sharedVariables = sharedVariables;

    for(size_t i = 0; i < variableCount; ++i)
    {
        crank_talk_oop_t sharedVariableDefinition = sharedVariablesDefinitionValues[i];
        crank_talk_oop_t sharedVariableValue = crank_talk_send0(primContext->context, primContext->context->roots.asClassVariableSymbol, sharedVariableDefinition);
        sharedVariables->elements[i] = sharedVariableValue;
    }

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_ShiftClassBuilder_sharedPools_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t sharedPoolsDefinitions = primContext->arguments[1];

    crank_talk_ShiftClassBuilder_t *classBuilder = (crank_talk_ShiftClassBuilder_t *)self;
    if(!crank_talk_oop_isKindOf(primContext->context, sharedPoolsDefinitions, primContext->context->roots.ArrayClass))
        crank_talk_error(primContext->context, "Expected an array with the slots.");

    classBuilder->sharedPools = (crank_talk_Array_t*)sharedPoolsDefinitions;

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_ShiftClassBuilder_tag_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    
    crank_talk_oop_t self = primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_ShiftClassBuilder_package_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    
    crank_talk_oop_t self = primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_ShiftClassBuilder_build_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];

    crank_talk_ShiftClassBuilder_t *classBuilder = (crank_talk_ShiftClassBuilder_t *)self;
    crank_talk_oop_t classValue = crank_talk_Behavior_createAnonymousClassAndMetaclass(primContext->context, classBuilder->superclass, 0, CrankTalkGCObjectDataTypeOops);
    crank_talk_Class_t *clazz = (crank_talk_Class_t *)classValue;
    clazz->super.super.slots = (crank_talk_oop_t)classBuilder->slots;
    clazz->subclasses = (crank_talk_Array_t*)crank_talk_Array_create(primContext->context, 0);
    clazz->name = classBuilder->name;
    clazz->environment = classBuilder->environment;
    clazz->classPool = (crank_talk_IdentityDictionary_t*)crank_talk_IdentityDictionary_create(primContext->context);
    clazz->sharedPools = classBuilder->sharedPools;

    size_t sharedVariableCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)classBuilder->sharedVariables);
    for(size_t i = 0; i < sharedVariableCount; ++i)
    {
        crank_talk_ClassVariable_t *classVariable = (crank_talk_ClassVariable_t*)classBuilder->sharedVariables->elements[i];
        crank_talk_IdentityDictionary_associationAtPut(primContext->context, (crank_talk_oop_t)clazz->classPool, (crank_talk_oop_t)classVariable);
    }

    crank_talk_oop_t metaclassValue = (crank_talk_oop_t)crank_talk_oop_getClass(primContext->context, classValue);
    assert(!crank_talk_oop_isNil(metaclassValue));
    crank_talk_Metaclass_t *metaClass = (crank_talk_Metaclass_t*)metaclassValue;
    if(crank_talk_oop_isNil(metaClass->super.super.slots))
        metaClass->super.super.slots = crank_talk_Array_create(primContext->context, 0);

    crank_talk_Behavior_createClassTableAndComputeLayouts(primContext->context, classValue);
    if(clazz->name && clazz->environment)
        crank_talk_SystemEnvironment_atPut(primContext->context, primContext->context->roots.systemEnvironment, (crank_talk_oop_t)clazz->name, classValue);

    return crank_talk_Primitive_return(primContext, classValue);
}

static void
crank_talk_Slot_asSlot_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_Symbol_asSlot_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t slotValue = CrankAllocateValuesObjectWithClassName(primContext->context, Slot);
    crank_talk_Slot_t *slot = (crank_talk_Slot_t *)slotValue;
    slot->super.name = (crank_talk_Symbol_t*)self;
    return crank_talk_Primitive_return(primContext, slotValue);
}

static void
crank_talk_ClassVariable_asClassVariable_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_Symbol_asClassVariable_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t classVariableValue = CrankAllocateValuesObjectWithClassName(primContext->context, ClassVariable);
    crank_talk_ClassVariable_t *classVariable = (crank_talk_ClassVariable_t *)classVariableValue;
    classVariable->super.super.name = (crank_talk_Symbol_t*)self;
    return crank_talk_Primitive_return(primContext, classVariableValue);
}

void
crank_talk_registerBehaviorPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "basicNew",  1, "Behavior::basicNew",  crank_talk_Behavior_basicNew_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "basicNew:", 2, "Behavior::basicNew:", crank_talk_Behavior_basicNewWith_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "new", 1,  "Behavior::new",  crank_talk_Behavior_new_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "new:", 2, "Behavior::new:", crank_talk_Behavior_newWith_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass,  "environment", 1, "Behavior::environment",  crank_talk_Behavior_environment_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ClassClass,     "environment", 1, "Class::environment",     crank_talk_Class_environment_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.MetaclassClass, "environment", 1, "Metaclass::environment", crank_talk_Metaclass_environment_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ClassClass,     "lookupClassBinding:", 2, "Class::lookupClassBinding:",     crank_talk_Class_lookupClassBinding_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.MetaclassClass, "lookupClassBinding:", 2, "Metaclass::lookupClassBinding:", crank_talk_Metaclass_lookupClassBinding_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BehaviorClass, "<<", 2, "Behavior::<<", crank_talk_Behavior_makeClassBuilder_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "slots:", 2, "ShiftClassBuilder::slots:", crank_talk_ShiftClassBuilder_slots_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "sharedPools:", 2, "ShiftClassBuilder::sharedPools:", crank_talk_ShiftClassBuilder_sharedPools_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "sharedVariables:", 2, "ShiftClassBuilder::sharedVariables:", crank_talk_ShiftClassBuilder_sharedVariables_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "tag:", 2, "ShiftClassBuilder::tag:", crank_talk_ShiftClassBuilder_tag_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "package:", 2, "ShiftClassBuilder::package:", crank_talk_ShiftClassBuilder_package_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ShiftClassBuilderClass, "build", 1, "ShiftClassBuilder::build", crank_talk_ShiftClassBuilder_build_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SlotClass,   "asSlot", 1, "Slot::asSlot",   crank_talk_Slot_asSlot_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SymbolClass, "asSlot", 1, "Symbol::asSlot", crank_talk_Symbol_asSlot_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ClassVariableClass,   "asClassVariable", 1, "ClassVariable::asClassVariable", crank_talk_ClassVariable_asClassVariable_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SymbolClass, "asClassVariable", 1, "Symbol::asClassVariable", crank_talk_Symbol_asClassVariable_primitive);
}

