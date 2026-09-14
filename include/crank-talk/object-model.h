#ifndef CRANK_TALK_OBJECT_MODEL_H
#define CRANK_TALK_OBJECT_MODEL_H

#include "common.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#pragma once

typedef intptr_t crank_talk_oop_t;

typedef struct crank_talk_VMContext_s crank_talk_VMContext_t;

typedef enum crank_talk_GCObjectDataType_e
{
    CrankTalkGCObjectDataTypeBytes = 0,
    CrankTalkGCObjectDataTypeOops,
    CrankTalkGCObjectDataTypeWeakOops,
    CrankTalkGCObjectDataTypeImmediate,
} crank_talk_GCObjectDataType_t;

enum crank_talk_oop_tag_e
{
    CRANK_TALK_OOP_TAG_BITS = 3,
    CRANK_TALK_OOP_TAG_MASK = (1<<CRANK_TALK_OOP_TAG_BITS) - 1,

    CRANK_TALK_OOP_TAG_POINTER_MASK = 7,
    CRANK_TALK_OOP_TAG_POINTER_VALUE = 0,
    CRANK_TALK_OOP_TAG_POINTER_SHIFT = 0,

    CRANK_TALK_OOP_TAG_SMALL_INTEGER_MASK = 7,
    CRANK_TALK_OOP_TAG_SMALL_INTEGER_VALUE = 1,
    CRANK_TALK_OOP_TAG_SMALL_INTEGER_SHIFT = 3,

    CRANK_TALK_OOP_TAG_CHARACTER_MASK = 7,
    CRANK_TALK_OOP_TAG_CHARACTER_VALUE = 2,
    CRANK_TALK_OOP_TAG_CHARACTER_SHIFT = 3,

    CRANK_TALK_OOP_TAG_SMALL_FLOAT_MASK = 7,
    CRANK_TALK_OOP_TAG_SMALL_FLOAT_VALUE = 4,
    CRANK_TALK_OOP_TAG_SMALL_FLOAT_SHIFT = 3,

    CRANK_TALK_OOP_BITS = 64,
};

#define CRANK_TALK_SMALL_FLOAT_EXPONENT_OFFSET (/* 1023 - 127 */ 896)
#define CRANK_TALK_SMALL_FLOAT_EXPONENT_MIN (/* 1023 - 127 */ CRANK_TALK_SMALL_FLOAT_EXPONENT_OFFSET)
#define CRANK_TALK_SMALL_FLOAT_EXPONENT_MAX (/* 1023 + 127 */ 1151)

#define CRANK_TALK_SMALL_INTEGER_USABLE_BITS (CRANK_TALK_OOP_BITS - CRANK_TALK_OOP_TAG_SMALL_INTEGER_SHIFT)
#define CRANK_TALK_SMALL_INTEGER_MIN_VALUE (-( ((intptr_t)1)<<(CRANK_TALK_SMALL_INTEGER_USABLE_BITS - 1) ) )
#define CRANK_TALK_SMALL_INTEGER_MAX_VALUE (+( ((intptr_t)1)<<(CRANK_TALK_SMALL_INTEGER_USABLE_BITS - 1) ) - 1)

#define CRANK_TALK_CHARACTER_USABLE_BITS (CRANK_TALK_OOP_BITS - CRANK_TALK_OOP_TAG_CHARACTER_SHIFT)
#define CRANK_TALK_CHARACTER_MAX_VALUE (+( ((intptr_t)1)<< CRANK_TALK_SMALL_INTEGER_USABLE_BITS) - 1)

#define CRANK_TALK_HASH_MULTIPLICATION_CONSTANT ((size_t)1103515245)
#define CRANK_TALK_HASH_BIT_COUNT (sizeof(uintptr_t)*8 - 5)
#define CRANK_TALK_HASH_BIT_MASK (((uintptr_t)1 << CRANK_TALK_HASH_BIT_COUNT) - 1)

#define CRANK_TALK_STORED_IDENTITY_HASH_BIT_COUNT 32
#define CRANK_TALK_STORED_IDENTITY_HASH_BIT_MASK 0xFFFFFFFF

CRANK_TALK_INLINE size_t
crank_talk_hashMultiply(size_t hash)
{
    return (hash * CRANK_TALK_HASH_MULTIPLICATION_CONSTANT) & CRANK_TALK_HASH_BIT_MASK;
}

CRANK_TALK_INLINE size_t
crank_talk_hashConcatenate(size_t previousHash, size_t nextHash)
{
    return (crank_talk_hashMultiply(previousHash) + nextHash) & CRANK_TALK_HASH_BIT_MASK;
}

typedef struct crank_talk_ObjectHeader_s crank_talk_ObjectHeader_t;

typedef struct crank_talk_ProtoObject_s crank_talk_ProtoObject_t;
    typedef struct crank_talk_Object_s crank_talk_Object_t;
    typedef struct crank_talk_Behavior_s crank_talk_Behavior_t;
        typedef struct crank_talk_ClassDescription_s crank_talk_ClassDescription_t;
            typedef struct crank_talk_Class_s crank_talk_Class_t;
            typedef struct crank_talk_Metaclass_s crank_talk_Metaclass_t;

    typedef struct crank_talk_CompiledFunction_s crank_talk_CompiledFunction_t;
    typedef struct crank_talk_BlockClosure_s crank_talk_BlockClosure_t;
    typedef struct crank_talk_Message_s crank_talk_Message_t;

    typedef struct crank_talk_Magnitude_s crank_talk_Magnitude_t;
        typedef struct crank_talk_Character_s crank_talk_Character_t;
        typedef struct crank_talk_Number_s crank_talk_Number_t;
            typedef struct crank_talk_Float_s crank_talk_Float_t;
                typedef struct crank_talk_BoxedFloat_s crank_talk_BoxedFloat_t;
                typedef struct crank_talk_SmallFloat_s crank_talk_SmallFloat_t;
            typedef struct crank_talk_Fraction_s crank_talk_Fraction_t;
            typedef struct crank_talk_Integer_s crank_talk_Integer_t;
                typedef struct crank_talk_LargeInteger_s crank_talk_LargeInteger_t;
                    typedef struct crank_talk_LargeNegativeInteger_s crank_talk_LargeNegativeInteger_t;
                    typedef struct crank_talk_LargePositiveInteger_s crank_talk_LargePositiveInteger_t;
                typedef struct crank_talk_SmallInteger_s crank_talk_SmallInteger_t;

    typedef struct crank_talk_UndefinedObject_s crank_talk_UndefinedObject_t;
    typedef struct crank_talk_Boolean_s crank_talk_Boolean_t;
        typedef struct crank_talk_False_s crank_talk_False_t;
        typedef struct crank_talk_True_s crank_talk_True_t;

    typedef struct crank_talk_Tombstone_s crank_talk_Tombstone_t;

    typedef struct crank_talk_Variable_s crank_talk_Variable_t;
        typedef struct crank_talk_LiteralVariable_s crank_talk_LiteralVariable_t;
            typedef struct crank_talk_GlobalVariable_s crank_talk_GlobalVariable_t;
            typedef struct crank_talk_LocalVariable_s crank_talk_LocalVariable_t;
        typedef struct crank_talk_HIRVariable_s crank_talk_HIRVariable_t;
        typedef struct crank_talk_Slot_s crank_talk_Slot_t;

    typedef struct crank_talk_LookupKey_s crank_talk_LookupKey_t;
        typedef struct crank_talk_Association_s crank_talk_Association_t;
 
    typedef struct crank_talk_SourceCode_s crank_talk_SourceCode_t;
    typedef struct crank_talk_AbstractSourcePosition_s crank_talk_AbstractSourcePosition_t;
        typedef struct crank_talk_EmptySourcePosition_s crank_talk_EmptySourcePosition_t;
        typedef struct crank_talk_SourcePosition_s crank_talk_SourcePosition_t;

    typedef struct crank_talk_CrankTalkToken_s crank_talk_CrankTalkToken_t;

    typedef struct crank_talk_Collection_s crank_talk_Collection_t;
        typedef struct crank_talk_HashedCollection_s crank_talk_HashedCollection_t;
            typedef struct crank_talk_Dictionary_s crank_talk_Dictionary_t;
                typedef struct crank_talk_IdentityDictionary_s crank_talk_IdentityDictionary_t;
                    typedef struct crank_talk_SystemEnvironment_s crank_talk_SystemEnvironment_t;
                typedef struct crank_talk_MethodDictionary_s crank_talk_MethodDictionary_t;
        
            typedef struct crank_talk_Set_s crank_talk_Set_t;
                typedef struct crank_talk_IdentitySet_s crank_talk_IdentitySet_t;
        
        typedef struct crank_talk_SequenceableCollection_s crank_talk_SequenceableCollection_t;
            typedef struct crank_talk_ArrayedCollection_s crank_talk_ArrayedCollection_t;
                typedef struct crank_talk_Array_s crank_talk_Array_t;
                typedef struct crank_talk_ByteArray_s crank_talk_ByteArray_t;
                typedef struct crank_talk_ValueArray_s crank_talk_ValueArray_t;
                typedef struct crank_talk_String_s crank_talk_String_t;
                typedef struct crank_talk_Symbol_s crank_talk_Symbol_t;
            typedef struct crank_talk_OrderedCollection_s crank_talk_OrderedCollection_t;

    typedef struct crank_talk_ByteStream_s crank_talk_ByteStream_t;
    typedef struct crank_talk_StringStream_s crank_talk_StringStream_t;

    typedef struct crank_talk_ExternalAddress_s crank_talk_ExternalAddress_t;

    typedef struct crank_talk_ShiftClassBuilder_s crank_talk_ShiftClassBuilder_t;

typedef crank_talk_oop_t (*crank_talk_CompiledFunctionNativeEntryPoint_t)(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments);

struct crank_talk_ObjectHeader_s {
    crank_talk_ObjectHeader_t *nextObject;
    crank_talk_Behavior_t* behavior;
    size_t byteSize;
    uint32_t identityHash;
    uint16_t padding;
    uint8_t dataType;
    uint8_t gcColor;
};

#define crank_talk_oop_nil() ((crank_talk_oop_t)0)

CRANK_TALK_INLINE bool
crank_talk_oop_isNil(crank_talk_oop_t oop)
{
    return oop == crank_talk_oop_nil();
}

CRANK_TALK_INLINE bool
crank_talk_oop_isPointers(crank_talk_oop_t oop)
{
    return (oop & CRANK_TALK_OOP_TAG_MASK) == CRANK_TALK_OOP_TAG_POINTER_VALUE;
}

CRANK_TALK_INLINE bool
crank_talk_oop_isNonNullPointer(crank_talk_oop_t oop)
{
    return oop != 0 && crank_talk_oop_isPointers(oop);
}

CRANK_TALK_INLINE bool
crank_talk_oop_isSmallInteger(crank_talk_oop_t oop)
{
    return (oop & CRANK_TALK_OOP_TAG_MASK) == CRANK_TALK_OOP_TAG_SMALL_INTEGER_VALUE;
}

CRANK_TALK_INLINE int
crank_talk_oop_isIntegerInSmallIntegerRange(intptr_t integer)
{
    return CRANK_TALK_SMALL_INTEGER_MIN_VALUE <= integer && integer <= CRANK_TALK_SMALL_INTEGER_MAX_VALUE;
}

CRANK_TALK_INLINE int
crank_talk_oop_isIntegerInCharacterRange(intptr_t integer)
{
    return 0 <= integer && integer <= CRANK_TALK_CHARACTER_MAX_VALUE;
}

CRANK_TALK_INLINE intptr_t
crank_talk_oop_decodeSmallInteger(crank_talk_oop_t oop)
{
    assert(crank_talk_oop_isSmallInteger(oop));
    return oop >> CRANK_TALK_OOP_TAG_SMALL_INTEGER_SHIFT;
}

CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_oop_encodeSmallInteger(intptr_t integer)
{
    assert(crank_talk_oop_isIntegerInSmallIntegerRange(integer));
    return (integer << CRANK_TALK_OOP_TAG_SMALL_INTEGER_SHIFT) | CRANK_TALK_OOP_TAG_SMALL_INTEGER_VALUE;
}

CRANK_TALK_INLINE bool
crank_talk_oop_isCharacter(crank_talk_oop_t oop)
{
    return (oop & CRANK_TALK_OOP_TAG_MASK) == CRANK_TALK_OOP_TAG_CHARACTER_VALUE;
}

CRANK_TALK_INLINE uintptr_t
crank_talk_oop_decodeCharacter(crank_talk_oop_t oop)
{
    assert(crank_talk_oop_isCharacter(oop));
    return oop >> CRANK_TALK_OOP_TAG_CHARACTER_SHIFT;
}

CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_oop_encodeCharacter(uintptr_t value)
{
    return (value << CRANK_TALK_OOP_TAG_CHARACTER_SHIFT) | CRANK_TALK_OOP_TAG_CHARACTER_VALUE;
}

CRANK_TALK_INLINE bool
crank_talk_oop_isSmallFloat(crank_talk_oop_t oop)
{
    return (oop & CRANK_TALK_OOP_TAG_MASK) == CRANK_TALK_OOP_TAG_SMALL_FLOAT_VALUE;
}

CRANK_TALK_INLINE bool
crank_talk_oop_isFloatInSmallFloatRange(double value)
{
    uint64_t valueIEEE754 = 0;
    memcpy(&valueIEEE754, &value, 8);

    uint64_t exponent = (valueIEEE754 >> 52) & 2047;
    return CRANK_TALK_SMALL_FLOAT_EXPONENT_MIN <= exponent && exponent <= CRANK_TALK_SMALL_FLOAT_EXPONENT_MAX;
}

CRANK_TALK_INLINE double
crank_talk_oop_decodeSmallFloat(crank_talk_oop_t oop)
{
    assert(crank_talk_oop_isSmallFloat(oop));

    // Shift out the tag.
    uint64_t decodedOop = (uintptr_t)oop >> 3;

    // Offset the exponent
    decodedOop += ((uint64_t)CRANK_TALK_SMALL_FLOAT_EXPONENT_OFFSET << 53);

    // Rotate right the sign bit.
    decodedOop = (decodedOop >> 1) | (decodedOop << 62);

    // Cast back to float.
    double floatValue = 0.0f;
    memcpy(&floatValue, &decodedOop, 8);
    return floatValue;
}

CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_oop_encodeSmallFloat(double value)
{
    assert(crank_talk_oop_isFloatInSmallFloatRange(value));

    uint64_t valueIEEE754 = 0;
    memcpy(&valueIEEE754, &value, 8);

    // Rotate left the sign.
    uintptr_t encodedOop = (valueIEEE754 << 1) | (valueIEEE754 >> 62);

    // Offset the exponent
    encodedOop -= ((uint64_t)CRANK_TALK_SMALL_FLOAT_EXPONENT_OFFSET << 53);

    // Add the tag.
    encodedOop = (encodedOop << CRANK_TALK_OOP_TAG_SMALL_FLOAT_SHIFT) | CRANK_TALK_OOP_TAG_SMALL_FLOAT_VALUE;

    return encodedOop;
}

CRANK_TALK_INLINE crank_talk_ObjectHeader_t *
crank_talk_object_getHeader(crank_talk_oop_t oop)
{
    assert(crank_talk_oop_isPointers(oop));
    return (crank_talk_ObjectHeader_t *)oop;
}

CRANK_TALK_INLINE crank_talk_Behavior_t *
crank_talk_object_getBehavior(crank_talk_oop_t oop)
{
    return crank_talk_object_getHeader(oop)->behavior;
}

CRANK_TALK_INLINE void
crank_talk_object_setBehavior(crank_talk_oop_t oop, crank_talk_Behavior_t *newType)
{
    crank_talk_object_getHeader(oop)->behavior = newType;
}

CRANK_TALK_INLINE size_t
crank_talk_object_getSizeInBytes(crank_talk_oop_t oop)
{
    if(!crank_talk_oop_isNonNullPointer(oop))
        return 0;

    crank_talk_ObjectHeader_t *header = crank_talk_object_getHeader(oop);
    return header->byteSize - sizeof(crank_talk_ObjectHeader_t);
}

CRANK_TALK_INLINE size_t
crank_talk_object_getSizeInSlots(crank_talk_oop_t oop)
{
    return crank_talk_object_getSizeInBytes(oop) / sizeof(crank_talk_oop_t);
}

CRANK_TALK_INLINE crank_talk_oop_t *
crank_talk_oop_getSlotsData(crank_talk_oop_t oop)
{
    return (crank_talk_oop_t*)(crank_talk_object_getHeader(oop) + 1);
}

CRANK_TALK_INLINE uint8_t *
crank_talk_oop_getBytesData(crank_talk_oop_t oop)
{
    return (uint8_t*)crank_talk_oop_getSlotsData(oop);
}

CRANK_TALK_INLINE uint32_t
crank_talk_object_getGCColor(crank_talk_oop_t oop)
{
    assert(crank_talk_oop_isNonNullPointer(oop));
    return crank_talk_object_getHeader(oop)->gcColor;
}

CRANK_TALK_INLINE void
crank_talk_object_setGCColor(crank_talk_oop_t oop, uint32_t newColor)
{
    assert(crank_talk_oop_isNonNullPointer(oop));
    crank_talk_object_getHeader(oop)->gcColor = newColor;
}

CRANK_TALK_INLINE void
crank_talk_object_setIdentityHash(crank_talk_oop_t oop, uint32_t newIdentityHash)
{
    assert(crank_talk_oop_isNonNullPointer(oop));
    crank_talk_object_getHeader(oop)->identityHash = newIdentityHash;
}

CRANK_TALK_INLINE size_t
crank_talk_oop_getIdentityHash(crank_talk_oop_t oop)
{
    if(crank_talk_oop_isNonNullPointer(oop))
        return crank_talk_object_getHeader(oop)->identityHash;
    return crank_talk_hashMultiply(oop);
}

struct crank_talk_ProtoObject_s {
    crank_talk_ObjectHeader_t header;
};

struct crank_talk_Object_s {
    crank_talk_ProtoObject_t super;
};

struct crank_talk_Behavior_s {
    crank_talk_Object_t super;
    crank_talk_Behavior_t *superclass;
    crank_talk_MethodDictionary_t *methodDictionary;
    crank_talk_oop_t instanceSize;
    crank_talk_oop_t instanceDataType;
    crank_talk_oop_t slots;
    crank_talk_oop_t allSlots;
};

struct crank_talk_ClassDescription_s {
    crank_talk_Behavior_t super;
};

struct crank_talk_Class_s {
    crank_talk_ClassDescription_t super;
    crank_talk_Symbol_t *name;
    crank_talk_oop_t environment;
};

struct crank_talk_Metaclass_s {
    crank_talk_ClassDescription_t super;
    crank_talk_Class_t *thisClass;
};

struct crank_talk_CompiledFunction_s {
    crank_talk_Object_t super;
    crank_talk_oop_t name;
    crank_talk_oop_t argumentCount;
    crank_talk_oop_t temporaryCount;
    crank_talk_ExternalAddress_t *nativeEntryPoint;
    crank_talk_ByteArray_t *bytecode;
    crank_talk_Array_t *literalVector;

    crank_talk_oop_t isBlock;
    crank_talk_oop_t definitionEnvironment;
    crank_talk_Array_t *definitionArguments;
    crank_talk_oop_t definitionBody;

    crank_talk_oop_t hirFunction;
};

struct crank_talk_BlockClosure_s {
    crank_talk_Object_t super;
    crank_talk_CompiledFunction_t *function;
    crank_talk_Array_t *captureVector;
};

struct crank_talk_Message_s {
    crank_talk_Object_t super;
    crank_talk_Symbol_t *selector;
    crank_talk_Array_t *arguments;
    crank_talk_Behavior_t *lookupClass;
};


struct crank_talk_Magnitude_s {
    crank_talk_Object_t super;
};

struct crank_talk_Character_s {
    crank_talk_Magnitude_t super;
};

struct crank_talk_Number_s {
    crank_talk_Magnitude_t super;
};

struct crank_talk_Float_s {
    crank_talk_Number_t super;
};

struct crank_talk_BoxedFloat_s {
    crank_talk_Float_t super;
    double value;
};

struct crank_talk_Fraction_s {
    crank_talk_Number_t super;
    crank_talk_oop_t numerator;
    crank_talk_oop_t denominator;
};

struct crank_talk_SmallFloat_s {
    crank_talk_Float_t super;
};

struct crank_talk_Integer_s {
    crank_talk_Number_t super;
};

struct crank_talk_LargeInteger_s {
    crank_talk_Integer_t super;
};

struct crank_talk_LargeNegativeInteger_s {
    crank_talk_LargeInteger_t super;
};

struct crank_talk_LargePositiveInteger_s {
    crank_talk_LargeInteger_t super;
};

struct crank_talk_SmallInteger_s {
    crank_talk_Integer_t super;
};

struct crank_talk_UndefinedObject_s {
    crank_talk_Object_t super;
};

struct crank_talk_Boolean_s {
    crank_talk_Object_t super;
};

struct crank_talk_False_s {
    crank_talk_Boolean_t super;
};

struct crank_talk_True_s {
    crank_talk_Boolean_t super;
};

struct crank_talk_Tombstone_s {
    crank_talk_Object_t super;
};

struct crank_talk_Variable_s {
    crank_talk_Object_t super;
    crank_talk_Symbol_t *name;
};

struct crank_talk_LiteralVariable_s {
    crank_talk_Variable_t super;
    crank_talk_oop_t value;
};

struct crank_talk_HIRVariable_s {
    crank_talk_Variable_t super;
    crank_talk_oop_t value;
};

struct crank_talk_GlobalVariable_s {
    crank_talk_LiteralVariable_t super;
};

struct crank_talk_LocalVariable_s {
    crank_talk_LiteralVariable_t super;
};

struct crank_talk_Slot_s {
    crank_talk_Variable_t super;
    crank_talk_oop_t index;
};

struct crank_talk_LookupKey_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t key;
};

struct crank_talk_Association_s
{
    crank_talk_LookupKey_t super;
    crank_talk_oop_t value;
};

struct crank_talk_SourceCode_s
{
    crank_talk_Object_t super;
    crank_talk_String_t *text;
    crank_talk_String_t *directory;
    crank_talk_String_t *name;
};

struct crank_talk_AbstractSourcePosition_s
{
    crank_talk_Object_t super;
};

struct crank_talk_EmptySourcePosition_s
{
    crank_talk_AbstractSourcePosition_t super;
};

struct crank_talk_SourcePosition_s
{
    crank_talk_AbstractSourcePosition_t super;
    crank_talk_SourceCode_t *sourceCode;
    
    crank_talk_oop_t startIndex;
    crank_talk_oop_t endIndex;
    crank_talk_oop_t startLine;
    crank_talk_oop_t endLine;
    crank_talk_oop_t startColumn;
    crank_talk_oop_t endColumn;
};

struct crank_talk_CrankTalkToken_s
{
    crank_talk_Object_t super;
    crank_talk_SourcePosition_t *sourcePosition;
    crank_talk_String_t *errorMessage;
    crank_talk_oop_t kind;
};

struct crank_talk_Collection_s
{
    crank_talk_Object_t super;
};

struct crank_talk_HashedCollection_s
{
    crank_talk_Collection_t super;
    crank_talk_oop_t size;
    crank_talk_Array_t *storage;
};

struct crank_talk_Dictionary_s
{
    crank_talk_HashedCollection_t super;
};

struct crank_talk_IdentityDictionary_s
{
    crank_talk_Dictionary_t super;
};

struct crank_talk_SystemEnvironment_s
{
    crank_talk_IdentityDictionary_t super;
};

struct crank_talk_MethodDictionary_s
{
    crank_talk_Dictionary_t super;
};

struct crank_talk_Set_s
{
    crank_talk_HashedCollection_t super;
};

struct crank_talk_IdentitySet_s
{
    crank_talk_Set_t super;
};

struct crank_talk_SequenceableCollection_s
{
    crank_talk_Collection_t super;
};

struct crank_talk_ArrayedCollection_s
{
    crank_talk_SequenceableCollection_t super;
};

struct crank_talk_Array_s
{
    crank_talk_ArrayedCollection_t super;
    crank_talk_oop_t elements[];
};

struct crank_talk_ByteArray_s
{
    crank_talk_ArrayedCollection_t super;
    uint8_t elements[];
};

struct crank_talk_String_s
{
    crank_talk_ArrayedCollection_t super;
    char elements[];
};

struct crank_talk_Symbol_s
{
    crank_talk_ArrayedCollection_t super;
    char elements[];
};

struct crank_talk_OrderedCollection_s
{
    crank_talk_SequenceableCollection_t super;
    crank_talk_oop_t size;
    crank_talk_Array_t *storage;
};

struct crank_talk_ByteStream_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t size;
    crank_talk_ByteArray_t *storage;
};

struct crank_talk_StringStream_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t size;
    crank_talk_String_t *storage;
};

struct crank_talk_ExternalAddress_s
{
    crank_talk_Object_t super;
    void* address;
};

struct crank_talk_ShiftClassBuilder_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t superclass;
    crank_talk_oop_t name;
    crank_talk_oop_t environment;
    crank_talk_oop_t slots;
};

CRANK_TALK_INLINE double
crank_talk_oop_asFloat(crank_talk_oop_t oop)
{
    if(crank_talk_oop_isSmallFloat(oop))
        return crank_talk_oop_decodeSmallFloat(oop);
    return ((crank_talk_BoxedFloat_t*)oop)->value;
}

CRANK_TALK_API double crank_talk_oop_numberAsFloat(crank_talk_oop_t oop);

CRANK_TALK_API bool crank_talk_oop_asBoolean(crank_talk_VMContext_t *context, crank_talk_oop_t value);
CRANK_TALK_API crank_talk_oop_t crank_talk_oop_fromBoolean(crank_talk_VMContext_t *context, bool value);
CRANK_TALK_API crank_talk_oop_t crank_talk_oop_fromFloat(crank_talk_VMContext_t *context, double value);
CRANK_TALK_API crank_talk_Behavior_t *crank_talk_oop_getClass(crank_talk_VMContext_t *context, crank_talk_oop_t value);

CRANK_TALK_API bool crank_talk_oop_isKindOf(crank_talk_VMContext_t *context, crank_talk_oop_t value, crank_talk_oop_t behavior);

#endif //CRANK_TALK_OBJECT_MODEL_H