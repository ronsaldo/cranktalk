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

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_concat(crank_talk_VMContext_t *context, crank_talk_oop_t leftString, crank_talk_oop_t rightString)
{
    size_t leftSize = crank_talk_object_getSizeInBytes(leftString);
    size_t rightSize = crank_talk_object_getSizeInBytes(rightString);
    crank_talk_oop_t resultValue = crank_talk_String_allocateWithSize(context, leftSize + rightSize);
    char *destination = (char*)crank_talk_oop_getBytesData(resultValue);
    memcpy(destination, crank_talk_oop_getBytesData(leftString), leftSize);
    memcpy(destination + leftSize, crank_talk_oop_getBytesData(rightString), rightSize);
    return resultValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_substring(crank_talk_VMContext_t *context, crank_talk_oop_t string, size_t startIndex, size_t count)
{
    crank_talk_oop_t result = crank_talk_String_allocateWithSize(context, count);
    const char *source = (char*)crank_talk_oop_getBytesData(string);
    char *destination = (char*)crank_talk_oop_getBytesData(result);
    memcpy(destination, source + startIndex, count);
    return result;
}

CRANK_TALK_API size_t
crank_talk_String_findLastSlash(crank_talk_oop_t path)
{
    size_t pathSize = crank_talk_object_getSizeInBytes(path);
    size_t bestFound = 0;
    char *data = (char*)crank_talk_oop_getBytesData(path);
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
    size_t stringSize = crank_talk_object_getSizeInBytes(path);
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


CRANK_TALK_API crank_talk_oop_t
crank_talk_StringStream_create(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t streamValue = CrankAllocateValuesObjectWithClassName(context, StringStream);
    crank_talk_StringStream_t *stream = (crank_talk_StringStream_t*)streamValue;
    stream->size = crank_talk_oop_encodeSmallInteger(0);
    return streamValue;
}

static void
crank_talk_StringStream_increaseCapacity(crank_talk_VMContext_t *context, crank_talk_StringStream_t *stream)
{
    size_t size = crank_talk_oop_decodeSmallInteger(stream->size);
    size_t capacity = crank_talk_object_getSizeInBytes((crank_talk_oop_t)stream->storage);
    size_t newCapacity = capacity*2;
    if(newCapacity < 16)
        newCapacity = 16;

    crank_talk_oop_t oldStorage = (crank_talk_oop_t)stream->storage;
    crank_talk_oop_t newStorage = CrankAllocateVariadicObjectWithClassName(context, String, CrankTalkGCObjectDataTypeBytes, newCapacity);
    memcpy(crank_talk_oop_getBytesData(newStorage), crank_talk_oop_getBytesData(oldStorage), size);
    stream->storage = (crank_talk_String_t*)newStorage;
}

CRANK_TALK_API void
crank_talk_StringStream_insertCharacter(crank_talk_VMContext_t *context, crank_talk_oop_t stream, char character)
{
    crank_talk_StringStream_t *stringStream = (crank_talk_StringStream_t*)stream;
    size_t size = crank_talk_oop_decodeSmallInteger(stringStream->size);
    size_t capacity = crank_talk_object_getSizeInBytes((crank_talk_oop_t)stringStream->storage);
    if(size >= capacity)
        crank_talk_StringStream_increaseCapacity(context, stringStream);

    crank_talk_String_t *storage = stringStream->storage;
    storage->elements[size] = character;
    stringStream->size = crank_talk_oop_encodeSmallInteger(size + 1);
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
    size_t size = crank_talk_object_getSizeInBytes(object);
    crank_talk_StringStream_insertString(context, stream, size, (const char*)crank_talk_oop_getBytesData(object));
}

/*CRANK_TALK_API void
crank_talk_StringStream_print(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t value)
{
    crank_talk_send1(context, context->roots.printOnSymbol, value, stream);
}

CRANK_TALK_API void
crank_talk_StringStream_fullPrint(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t value)
{
    crank_talk_send1(context, context->roots.fullPrintOnSymbol, value, stream);
}*/

CRANK_TALK_API crank_talk_oop_t
crank_talk_StringStream_asString(crank_talk_VMContext_t *context, crank_talk_oop_t stream)
{
    crank_talk_StringStream_t *stringStream = (crank_talk_StringStream_t*)stream;
    size_t symbolSize = crank_talk_oop_decodeSmallInteger(stringStream->size);
    return crank_talk_String_createWithString(context, symbolSize, stringStream->storage->elements);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_StringStream_asSymbol(crank_talk_VMContext_t *context, crank_talk_oop_t stream)
{
    crank_talk_StringStream_t *stringStream = (crank_talk_StringStream_t*)stream;
    size_t symbolSize = crank_talk_oop_decodeSmallInteger(stringStream->size);
    return crank_talk_Symbol_internWithString(context, symbolSize, stringStream->storage->elements);
}

/*
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
*/
