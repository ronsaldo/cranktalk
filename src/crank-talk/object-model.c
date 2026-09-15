#include "crank-talk/object-model.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/gc.h"
#include "crank-talk/message.h"
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

CRANK_TALK_API bool
crank_talk_oop_asBoolean(crank_talk_VMContext_t *context, crank_talk_oop_t value)
{
    return value != context->roots.falseValue;
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

CRANK_TALK_API crank_talk_oop_t
crank_talk_ExternalAddress_make(crank_talk_VMContext_t *context, void *pointer)
{
    crank_talk_ExternalAddress_t *entryPointAddress = (crank_talk_ExternalAddress_t*)CrankAllocateBytesObjectWithClassName(context, ExternalAddress);
    entryPointAddress->address = pointer;
    return (crank_talk_oop_t)entryPointAddress;
}

static crank_talk_oop_t
crank_talk_ProtoObject_class_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    return (crank_talk_oop_t)crank_talk_oop_getClass(context, arguments[0]);
}

static crank_talk_oop_t
crank_talk_Object_species_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    return (crank_talk_oop_t)crank_talk_oop_getClass(context, arguments[0]);
}

static crank_talk_oop_t
crank_talk_ProtoObject_yourself_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ProtoObject_identityEquals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];
    return crank_talk_oop_fromBoolean(context, self == operand);
}

static crank_talk_oop_t
crank_talk_ProtoObject_identityNotEquals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];
    return crank_talk_oop_fromBoolean(context, self != operand);
}

static crank_talk_oop_t
crank_talk_ProtoObject_identityHash_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t self = arguments[0];
    return crank_talk_oop_encodeSmallInteger(crank_talk_oop_getIdentityHash(self));
}

static crank_talk_oop_t
crank_talk_ProtoObject_initialize_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t self = arguments[0];
    return self;
}

static crank_talk_oop_t
crank_talk_Object_equals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];
    return crank_talk_oop_fromBoolean(context, self == operand);
}

static crank_talk_oop_t
crank_talk_Object_notEquals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];
    crank_talk_oop_t isEquals = crank_talk_send1(context, context->roots.equalsSymbol, self, operand);
    if(isEquals == context->roots.trueValue)
    {
        return context->roots.falseValue;
    }
    else if(isEquals == context->roots.falseValue)
    {
        return context->roots.trueValue;
    }
    else
    {
        crank_talk_error("Expected a boolean result for ~=.");
        abort();
    }
}

static crank_talk_oop_t
crank_talk_Object_hash_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t self = arguments[0];
    return crank_talk_oop_encodeSmallInteger(crank_talk_oop_getIdentityHash(self));
}

static crank_talk_oop_t
crank_talk_Object_basicSize_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);
    crank_talk_oop_t self = arguments[0];
    if(!crank_talk_oop_isNonNullPointer(self))
        return crank_talk_oop_encodeSmallInteger(0);

    crank_talk_ObjectHeader_t *header = crank_talk_object_getHeader(self);
    size_t classSize = crank_talk_oop_decodeSmallInteger(header->behavior->instanceSize);
    assert(header->byteSize >= classSize);
    size_t variableByteSize = header->byteSize - classSize;
    if(header->dataType == CrankTalkGCObjectDataTypeBytes)
        return crank_talk_oop_encodeSmallInteger(variableByteSize);
    else
        return crank_talk_oop_encodeSmallInteger(variableByteSize / sizeof(crank_talk_oop_t));
}

static crank_talk_oop_t
crank_talk_Object_basicAt_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t indexValue = arguments[1];

    if(!crank_talk_oop_isNonNullPointer(self))
        crank_talk_error("Expected a non-immediate object.");
    if(!crank_talk_oop_isSmallInteger(indexValue))
        crank_talk_error("Expected a small integer index.");

    size_t index = crank_talk_oop_decodeSmallInteger(indexValue);

    crank_talk_ObjectHeader_t *header = crank_talk_object_getHeader(self);
    size_t classSize = crank_talk_oop_decodeSmallInteger(header->behavior->instanceSize);
    assert(header->byteSize >= classSize);

    size_t variableByteSize = header->byteSize - classSize;
    uint8_t *variableByteData = (uint8_t*)header + classSize;

    if(header->dataType == CrankTalkGCObjectDataTypeBytes)
    {
        if(index < 1 || index > variableByteSize)
            crank_talk_error("Index is out of bounds.");
        return crank_talk_oop_encodeSmallInteger(variableByteData[index - 1]);
    }
    else
    {
        size_t variableSlotSize = variableByteSize / sizeof(crank_talk_oop_t);
        crank_talk_oop_t *variableSlotData = (crank_talk_oop_t *)variableByteData;
        if(index < 1 || index > variableSlotSize)
            crank_talk_error("Index is out of bounds.");
        return variableSlotData[index - 1];
    }
}

static crank_talk_oop_t
crank_talk_Object_basicAtPut_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 3);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t indexValue = arguments[1];
    crank_talk_oop_t value = arguments[2];

    if(!crank_talk_oop_isNonNullPointer(self))
        crank_talk_error("Expected a non-immediate object.");
    if(!crank_talk_oop_isSmallInteger(indexValue))
        crank_talk_error("Expected a small integer index.");

    size_t index = crank_talk_oop_decodeSmallInteger(indexValue);

    crank_talk_ObjectHeader_t *header = crank_talk_object_getHeader(self);
    size_t classSize = crank_talk_oop_decodeSmallInteger(header->behavior->instanceSize);
    assert(header->byteSize >= classSize);

    size_t variableByteSize = header->byteSize - classSize;
    uint8_t *variableByteData = (uint8_t*)header + classSize;

    if(header->dataType == CrankTalkGCObjectDataTypeBytes)
    {
        if(index < 1 || index > variableByteSize)
            crank_talk_error("Index is out of bounds.");
        if(!crank_talk_oop_isSmallInteger(value))
            crank_talk_error("Expected an integer value.");

        variableByteData[index - 1] = crank_talk_oop_decodeSmallInteger(value);
    }
    else
    {
        size_t variableSlotSize = variableByteSize / sizeof(crank_talk_oop_t);
        crank_talk_oop_t *variableSlotData = (crank_talk_oop_t *)variableByteData;
        if(index < 1 || index > variableSlotSize)
            crank_talk_error("Index is out of bounds.");
        variableSlotData[index - 1] = value;
    }

    return value;
}

static crank_talk_oop_t
crank_talk_SmalltalkImage_garbageCollect_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);
    crank_talk_gc_collect(context);
    return arguments[0];
}

static crank_talk_oop_t
crank_talk_SmalltalkImage_globals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);
    crank_talk_SmalltalkImage_t *smalltalk = (crank_talk_SmalltalkImage_t *)arguments[0];
    return (crank_talk_oop_t)smalltalk->globals;
}

void
crank_talk_registerObjectPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ProtoObjectClass, "class", 1, crank_talk_ProtoObject_class_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ProtoObjectClass, "==", 2, crank_talk_ProtoObject_identityEquals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ProtoObjectClass, "~~", 2, crank_talk_ProtoObject_identityNotEquals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ProtoObjectClass, "identityHash", 2, crank_talk_ProtoObject_identityHash_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ProtoObjectClass, "initialize", 1, crank_talk_ProtoObject_initialize_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ProtoObjectClass, "yourself", 1, crank_talk_ProtoObject_yourself_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "=", 2, crank_talk_Object_equals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "~=", 2, crank_talk_Object_notEquals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "hash", 2, crank_talk_Object_hash_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "species", 1, crank_talk_Object_species_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "basicSize", 1, crank_talk_Object_basicSize_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "size", 1, crank_talk_Object_basicSize_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "basicAt:", 1, crank_talk_Object_basicAt_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "basicAt:put:", 2, crank_talk_Object_basicAtPut_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "at:", 1, crank_talk_Object_basicAt_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "at:put:", 2, crank_talk_Object_basicAtPut_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmalltalkImageClass, "garbageCollect", 1, crank_talk_SmalltalkImage_garbageCollect_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmalltalkImageClass, "globals", 1, crank_talk_SmalltalkImage_globals_primitive);
}