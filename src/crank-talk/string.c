#include "crank-talk/string.h"
#include "crank-talk/context.h"
#include "crank-talk/set.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct crank_talk_stringSlice_s
{
    const char *elements;
    size_t size;
} crank_talk_stringSlice_t;

static bool
crank_talk_stringSlice_equalsFunction(void *element, crank_talk_oop_t setElement)
{
    crank_talk_stringSlice_t *stringSlice = (crank_talk_stringSlice_t*)element;

    size_t setSymbolSize = crank_talk_object_getSizeInBytes(setElement);
    if(stringSlice->size != setSymbolSize)
        return false;

    return memcmp(stringSlice->elements, crank_talk_oop_getBytesData(setElement), setSymbolSize) == 0;
}

static size_t
crank_talk_stringSliceSymbol_hashFunction(crank_talk_VMContext_t *context, void *element)
{
    (void)context;
    crank_talk_stringSlice_t *stringSlice = (crank_talk_stringSlice_t*)element;
    return crank_talk_String_computeHashWithBytes(stringSlice->size, (const uint8_t *)stringSlice->elements) & CRANK_TALK_STORED_IDENTITY_HASH_BIT_MASK;
}

CRANK_TALK_API size_t
crank_talk_String_computeHashWithBytes(size_t size, const uint8_t *bytes)
{
    size_t result = crank_talk_hashMultiply(1);
    for(size_t i = 0; i < size; ++i)
        result = crank_talk_hashConcatenate(result, bytes[i]);
    return result;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_allocateWithSize(crank_talk_VMContext_t *context, size_t stringSize)
{
    return CrankAllocateVariadicObjectWithClassName(context, String, CrankTalkGCObjectDataTypeBytes, stringSize);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_createWithString(crank_talk_VMContext_t *context, size_t stringSize, const char *string)
{
    crank_talk_oop_t stringValue = crank_talk_String_allocateWithSize(context, stringSize);
    memcpy(crank_talk_oop_getBytesData(stringValue), string, stringSize);
    return stringValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_createWithCString(crank_talk_VMContext_t *context, const char *cstring)
{
    return crank_talk_String_createWithString(context, strlen(cstring), cstring);
}

CRANK_TALK_API bool
crank_talk_String_equalsCString(crank_talk_oop_t left, const char *right)
{
    size_t leftSize = crank_talk_object_getSizeInBytes(left);
    size_t rightSize = strlen(right);
    if(leftSize != rightSize)
        return false;
    
    return memcmp(crank_talk_oop_getBytesData(left), right, leftSize) == 0;
}

/*
CRANK_TALK_API crank_talk_oop_t
crank_talk_String_concat(crank_talk_VMContext_t *context, crank_talk_oop_t leftString, crank_talk_oop_t rightString)
{
    size_t leftSize = crank_talk_Value_getSizeInBytes(leftString);
    size_t rightSize = crank_talk_Value_getSizeInBytes(rightString);
    crank_talk_oop_t resultValue = crank_talk_String_allocateWithSize(context, leftSize + rightSize);
    char *destination = crank_talk_Value_getBytesData(resultValue);
    memcpy(destination, crank_talk_Value_getBytesData(leftString), leftSize);
    memcpy(destination + leftSize, crank_talk_Value_getBytesData(rightString), rightSize);
    return resultValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_substring(crank_talk_VMContext_t *context, crank_talk_oop_t string, size_t startIndex, size_t count)
{
    crank_talk_oop_t result = crank_talk_String_allocateWithSize(context, count);
    const char *source = crank_talk_Value_getBytesData(string);
    char *destination = crank_talk_Value_getBytesData(result);
    memcpy(destination, source + startIndex, count);
    return result;
}

CRANK_TALK_API size_t
crank_talk_String_findLastSlash(crank_talk_oop_t path)
{
    size_t pathSize = crank_talk_Value_getSizeInBytes(path);
    size_t bestFound = 0;
    char *data = crank_talk_Value_getBytesData(path);
    for(size_t i = 0; i < pathSize; ++i)
    {
        char c = data[i];
        if(c == '/' || c == '\\')
            bestFound = i;
    }
    return bestFound;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_dirname(crank_talk_VMContext_t *context, crank_talk_oop_t path)
{
    size_t lastSlachPosition = crank_talk_String_findLastSlash(path);
    return crank_talk_String_substring(context, path, 0, lastSlachPosition);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_basename(crank_talk_VMContext_t *context, crank_talk_oop_t path)
{
    size_t stringSize = crank_talk_Value_getSizeInBytes(path);
    size_t lastSlachPosition = crank_talk_String_findLastSlash(path);
    if(lastSlachPosition >= stringSize)
        return crank_talk_String_createWithCString(context, "");
    size_t count = stringSize - lastSlachPosition - 1;
    return crank_talk_String_substring(context, path, lastSlachPosition + 1, count);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_joinPath(crank_talk_VMContext_t *context, crank_talk_oop_t leftPath, crank_talk_oop_t rightPath)
{
    // TODO: Handle absolute paths.
    crank_talk_oop_t result = leftPath;
    result = crank_talk_String_concat(context, result, crank_talk_String_createWithCString(context, "/"));
    result = crank_talk_String_concat(context, result, rightPath);
    return result;
}
*/

CRANK_TALK_API crank_talk_oop_t
crank_talk_Symbol_internWithString(crank_talk_VMContext_t *context, size_t stringSize, const char *stringData)
{
    // Find an already interned symbol.
    {
        crank_talk_stringSlice_t stringSlice = {
            .elements = stringData,
            .size = stringSize
        };

        crank_talk_oop_t existent = crank_talk_oop_nil();
        if(crank_talk_IdentitySet_findWithExplicitHash(context, context->roots.internedSymbolSet, &stringSlice, crank_talk_stringSliceSymbol_hashFunction, crank_talk_stringSlice_equalsFunction, &existent))
            return existent;
    }

    // Create the symbol.
    crank_talk_oop_t symbolValue = CrankAllocateVariadicObjectWithClassName(context, Symbol, CrankTalkGCObjectDataTypeBytes, stringSize);
    crank_talk_object_getHeader(symbolValue)->identityHash = (uint32_t)crank_talk_String_computeHashWithBytes(stringSize, (const uint8_t*)stringData);
    memcpy(crank_talk_oop_getBytesData(symbolValue), stringData, stringSize);

    // Intern the symbol in a set.
    crank_talk_IdentitySet_insert(context, context->roots.internedSymbolSet, symbolValue);
    return symbolValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Symbol_internWithCString(crank_talk_VMContext_t *context, const char *cstring)
{
    return crank_talk_Symbol_internWithString(context, strlen(cstring), cstring);
}

CRANK_TALK_API bool crank_talk_Symbol_equalsCString(crank_talk_oop_t left, const char *right)
{
    return crank_talk_String_equalsCString(left, right);
}

/*
CRANK_TALK_API crank_talk_oop_t
crank_talk_StringStream_create(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t streamValue = SysmelAllocateValuesObjectWithClassName(context, StringStream);
    crank_talk_StringStream_t *stream = (crank_talk_StringStream_t*)streamValue.value.gc;
    stream->size = crank_talk_Value_forSize(0);
    return streamValue;
}

static void
crank_talk_StringStream_increaseCapacity(crank_talk_VMContext_t *context, crank_talk_StringStream_t *stream)
{
    size_t size = crank_talk_Value_getSize(stream->size);
    size_t capacity = crank_talk_Value_getSizeInBytes(stream->storage);
    size_t newCapacity = capacity*2;
    if(newCapacity < 16)
        newCapacity = 16;

    crank_talk_oop_t oldStorage = stream->storage;;
    crank_talk_oop_t newStorage = SysmelAllocateVariadicObjectWithClassName(context, String, SysmelGCObjectDataTypeBytes, newCapacity);
    memcpy(crank_talk_Value_getBytesData(newStorage), crank_talk_Value_getBytesData(oldStorage), size);
    stream->storage = newStorage;
}


CRANK_TALK_API void
crank_talk_StringStream_insertCharacter(crank_talk_VMContext_t *context, crank_talk_oop_t stream, char character)
{
    crank_talk_StringStream_t *stringStream = (crank_talk_StringStream_t*)stream.value.gc;
    size_t size = crank_talk_Value_getSize(stringStream->size);
    size_t capacity = crank_talk_Value_getSizeInBytes(stringStream->storage);
    if(size >= capacity)
        crank_talk_StringStream_increaseCapacity(context, stringStream);

    crank_talk_String_t *storage = (crank_talk_String_t*)stringStream->storage.value.gc;
    storage->elements[size] = character;
    stringStream->size = crank_talk_Value_forSize(size + 1);
}

CRANK_TALK_API void
crank_talk_StringStream_insertString(crank_talk_VMContext_t *context, crank_talk_oop_t stream, size_t stringSize, const char *stringData)
{
    for(size_t i = 0; i < stringSize; ++i)
        crank_talk_StringStream_insertCharacter(context, stream, stringData[i]);
}

CRANK_TALK_API void
crank_talk_StringStream_insertCString(crank_talk_VMContext_t *context, crank_talk_oop_t stream, const char *string)
{
    crank_talk_StringStream_insertString(context, stream, strlen(string), string);
}

CRANK_TALK_API void
crank_talk_StringStream_insertBytesObject(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t object)
{
    size_t size = crank_talk_Value_getSizeInBytes(object);
    crank_talk_StringStream_insertString(context, stream, size, crank_talk_Value_getBytesData(object));
}

CRANK_TALK_API void
crank_talk_StringStream_print(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t value)
{
    crank_talk_send1(context, context->roots.printOnSymbol, value, stream);
}

CRANK_TALK_API void
crank_talk_StringStream_fullPrint(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t value)
{
    crank_talk_send1(context, context->roots.fullPrintOnSymbol, value, stream);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_StringStream_asString(crank_talk_VMContext_t *context, crank_talk_oop_t stream)
{
    crank_talk_StringStream_t *stringStream = (crank_talk_StringStream_t*)stream.value.gc;
    size_t symbolSize = crank_talk_Value_getSize(stringStream->size);
    return crank_talk_String_createWithString(context, symbolSize, crank_talk_Value_getBytesData(stringStream->storage));
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_StringStream_asSymbol(crank_talk_VMContext_t *context, crank_talk_oop_t stream)
{
    crank_talk_StringStream_t *stringStream = (crank_talk_StringStream_t*)stream.value.gc;
    size_t symbolSize = crank_talk_Value_getSize(stringStream->size);
    return crank_talk_Symbol_internWithString(context, symbolSize, crank_talk_Value_getBytesData(stringStream->storage));
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Value_asString(crank_talk_VMContext_t *context, crank_talk_oop_t value)
{
    return crank_talk_send0(context, context->roots.asStringSymbol, value);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Value_printString(crank_talk_VMContext_t *context, crank_talk_oop_t value)
{
    return crank_talk_send0(context, context->roots.printStringSymbol, value);
}

static crank_talk_oop_t
crank_talk_Object_asString_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    return crank_talk_send0(context, context->roots.printStringSymbol, arguments[0]);
}

static crank_talk_oop_t
crank_talk_Object_printString_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = crank_talk_StringStream_create(context);
    crank_talk_StringStream_print(context, stream, receiver);
    return crank_talk_StringStream_asString(context, stream);
}

static crank_talk_oop_t
crank_talk_Object_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCString(context, stream, "a ");

    crank_talk_oop_t receiverType = crank_talk_Value_getType(context, receiver);
    crank_talk_StringStream_print(context, stream, receiverType);
    
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_Object_fullPrintOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_print(context, stream, receiver);
    
    return crank_talk_Value_void();
}


static crank_talk_oop_t
crank_talk_Type_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCString(context, stream, "a Type");
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_NominalType_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_NominalType_t *nominalType = (crank_talk_NominalType_t *)arguments[0].value.gc;
    crank_talk_oop_t stream = arguments[1];
    if(!crank_talk_Value_isNil(nominalType->name))
        crank_talk_StringStream_insertBytesObject(context, stream, nominalType->name);
    else
        crank_talk_StringStream_insertCString(context, stream, "a NominalType");
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_Metaclass_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_Metaclass_t *metaclass = (crank_talk_Metaclass_t *)arguments[0].value.gc;
    crank_talk_oop_t stream = arguments[1];
    if(!crank_talk_Value_isNil(metaclass->thisClass))
    {
        crank_talk_StringStream_print(context, stream, metaclass->thisClass);
        crank_talk_StringStream_insertCString(context, stream, " class");
    }
    else
    {
        crank_talk_StringStream_insertCString(context, stream, "a Metaclass");
    }
        
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_SimpleFunctionType_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_SimpleFunctionType_t *functionType = (crank_talk_SimpleFunctionType_t *)arguments[0].value.gc;
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_StringStream_insertCString(context, stream, "(");

    size_t functionArgumentCount = crank_talk_Value_getSizeInValues(functionType->argumentTypes);
    crank_talk_oop_t *functionArguments = crank_talk_Value_getValuesData(functionType->argumentTypes);
    for(size_t i = 0; i < functionArgumentCount; ++i)
    {
        if(i > 0)
            crank_talk_StringStream_insertCString(context, stream, ", ");
        crank_talk_StringStream_print(context, stream, functionArguments[i]);
    }

    crank_talk_StringStream_insertCString(context, stream, ") => ");
    crank_talk_StringStream_print(context, stream, functionType->resultType);
        
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_ArgumentDefinition_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ArgumentDefinition_t *argumentDefinition = (crank_talk_ArgumentDefinition_t *)arguments[0].value.gc;
    crank_talk_oop_t stream = arguments[1];

    crank_talk_StringStream_insertCString(context, stream, ":");
    if(!crank_talk_Value_isNil(argumentDefinition->typeExpression))
    {
        crank_talk_StringStream_insertCString(context, stream, "(");
        crank_talk_StringStream_print(context, stream, argumentDefinition->typeExpression);
        crank_talk_StringStream_insertCString(context, stream, ")");
    }

    if(!crank_talk_Value_isNil(argumentDefinition->name))
        crank_talk_StringStream_insertBytesObject(context, stream, argumentDefinition->name);

    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_DependentFunctionType_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_DependentFunctionType_t *functionType = (crank_talk_DependentFunctionType_t *)arguments[0].value.gc;
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_StringStream_insertCString(context, stream, "{");

    size_t functionArgumentCount = crank_talk_Value_getSizeInValues(functionType->arguments);
    crank_talk_oop_t *functionArguments = crank_talk_Value_getValuesData(functionType->arguments);
    for(size_t i = 0; i < functionArgumentCount; ++i)
    {
        if(i > 0)
            crank_talk_StringStream_insertCString(context, stream, " ");
        crank_talk_StringStream_print(context, stream, functionArguments[i]);
    }

    if(!crank_talk_Value_isNil(functionType->resultTypeExpression))
    {
        crank_talk_StringStream_insertCString(context, stream, " :: "); 
        crank_talk_StringStream_print(context, stream, functionType->resultTypeExpression);   
    }
        
    crank_talk_StringStream_insertCString(context, stream, "}");
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_PointerType_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_PointerType_t *pointerType = (crank_talk_PointerType_t*)arguments[0].value.gc;
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_StringStream_print(context, stream, pointerType->super.super.baseType);
    crank_talk_StringStream_insertCString(context, stream, " pointer");

    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_ReferenceType_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ReferenceType_t *referenceType = (crank_talk_ReferenceType_t*)arguments[0].value.gc;
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_StringStream_print(context, stream, referenceType->super.super.baseType);
    crank_talk_StringStream_insertCString(context, stream, " ref");

    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_UndefinedObject_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    //crank_talk_oop_t voidValue = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_StringStream_insertCString(context, stream, "nil");
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_Void_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    //crank_talk_oop_t voidValue = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_StringStream_insertCString(context, stream, "void");
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_DynamicType_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    //crank_talk_oop_t voidValue = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_StringStream_insertCString(context, stream, "Dynamic");
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_String_asString_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    return arguments[0];
}

static crank_talk_oop_t
crank_talk_String_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCharacter(context, stream, '"');
    crank_talk_StringStream_insertBytesObject(context, stream, self);
    crank_talk_StringStream_insertCharacter(context, stream, '"');
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_Symbol_asString_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t receiver = arguments[0];
    return crank_talk_String_createWithString(context, crank_talk_Value_getSizeInBytes(receiver), crank_talk_Value_getBytesData(receiver));
}

static crank_talk_oop_t
crank_talk_Symbol_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCharacter(context, stream, '#');
    crank_talk_StringStream_insertBytesObject(context, stream, self);
    return crank_talk_Value_void();
}

static crank_talk_oop_t
crank_talk_EnumValue_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_EnumValue_t *enumValue = (crank_talk_EnumValue_t *)arguments[0].value.gc;
    crank_talk_oop_t stream = arguments[1];
    if(!crank_talk_Value_isNil(enumValue->name))
    {
        crank_talk_StringStream_insertBytesObject(context, stream, enumValue->name);
    }
    else
    {
        crank_talk_StringStream_print(context, stream, enumValue->value);
    }
        
    return crank_talk_Value_void();
}

void
crank_talk_String_registerPrimitives(crank_talk_VMContext_t *context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "asString", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(StringClass), SysmelCurrentTypePointer(ObjectClass), NULL), crank_talk_Object_asString_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "printString", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(StringClass), SysmelCurrentTypePointer(ObjectClass), NULL), crank_talk_Object_printString_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(ObjectClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_Object_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "fullPrintOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(ObjectClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_Object_fullPrintOn_primitive);
    
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TypeClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(TypeClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_Type_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.NominalTypeClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(NominalTypeClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_NominalType_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.MetaclassClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(MetaclassClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_Metaclass_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ArgumentDefinitionClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(ArgumentDefinitionClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_ArgumentDefinition_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.DependentFunctionTypeClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(DependentFunctionTypeClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_DependentFunctionType_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SimpleFunctionTypeClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(SimpleFunctionTypeClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_SimpleFunctionType_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.PointerTypeClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(PointerTypeClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_PointerType_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ReferenceTypeClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(ReferenceTypeClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_ReferenceType_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.UndefinedObjectClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_UndefinedObject_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.VoidClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_Void_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.DynamicTypeClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(DynamicTypeClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_DynamicType_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringClass, "asString", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsCompileTimePureFinal, SysmelCurrentTypePointer(StringClass), SysmelCurrentTypePointer(StringClass), NULL), crank_talk_String_asString_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(StringClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_String_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SymbolClass, "asString", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsCompileTimePureFinal, SysmelCurrentTypePointer(StringClass), SysmelCurrentTypePointer(SymbolClass), NULL), crank_talk_Symbol_asString_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SymbolClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(SymbolClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_Symbol_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.EnumValueClass, "printOn:", crank_talk_SimpleFunctionType_createf(context, SysmelVMFunctionFlagsNone, SysmelCurrentTypePointer(VoidClass), SysmelCurrentTypePointer(EnumValueClass), SysmelCurrentTypePointer(StringStreamClass), NULL), crank_talk_EnumValue_printOn_primitive);

}
*/