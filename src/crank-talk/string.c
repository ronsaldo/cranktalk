#include "crank-talk/string.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
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
crank_talk_String_withoutElementAtIndex(crank_talk_VMContext_t *context, crank_talk_oop_t string, size_t removedIndex)
{
    size_t stringSize = crank_talk_object_getSizeInBytes(string);
    if(removedIndex >= stringSize)
        return string;

    size_t stringWithoutElementSize = stringSize - 1;
    crank_talk_oop_t result = crank_talk_String_allocateWithSize(context, stringWithoutElementSize);
    crank_talk_String_t *sourceString = (crank_talk_String_t *)string;
    crank_talk_String_t *resultString = (crank_talk_String_t *)result;
    memcpy(resultString->elements, sourceString->elements, removedIndex);
    memcpy(resultString->elements + removedIndex, sourceString->elements + removedIndex + 1, stringSize - removedIndex - 1);

    return result;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_String_insertStringAtIndex(crank_talk_VMContext_t *context, crank_talk_oop_t string, crank_talk_oop_t insertedString, size_t insertionIndex)
{
    size_t stringSize = crank_talk_object_getSizeInBytes(string);
    if(insertionIndex >= stringSize)
        return crank_talk_String_concat(context, string, insertedString);
    
    size_t insertedStringSize = crank_talk_object_getSizeInBytes(insertedString);
    size_t totalSize = stringSize + insertedStringSize;
    crank_talk_oop_t result = crank_talk_String_allocateWithSize(context, totalSize);

    crank_talk_String_t *sourceStringValue = (crank_talk_String_t *)string;
    crank_talk_String_t *insertedStringValue = (crank_talk_String_t *)insertedString;
    crank_talk_String_t *resultStringValue = (crank_talk_String_t *)result;
    memcpy(resultStringValue->elements, sourceStringValue->elements, insertionIndex);
    memcpy(resultStringValue->elements + insertionIndex, insertedStringValue->elements, insertedStringSize);
    memcpy(resultStringValue->elements + insertionIndex + insertedStringSize, sourceStringValue->elements + insertionIndex, stringSize - insertionIndex);
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

CRANK_TALK_API crank_talk_oop_t
crank_talk_oop_asString(crank_talk_VMContext_t *context, crank_talk_oop_t value)
{
    return crank_talk_send0(context, context->roots.asStringSymbol, value);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_oop_printString(crank_talk_VMContext_t *context, crank_talk_oop_t value)
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
    assert(argumentCount > 0);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCString(context, stream, "a ");

    crank_talk_oop_t receiverClass = (crank_talk_oop_t)crank_talk_oop_getClass(context, receiver);
    crank_talk_StringStream_print(context, stream, receiverClass);

    return receiver;
}

static crank_talk_oop_t
crank_talk_Object_fullPrintOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_print(context, stream, receiver);
    return receiver;
}

static crank_talk_oop_t
crank_talk_Class_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_Class_t *clazz = (crank_talk_Class_t*)receiver;
    if(!clazz->name)
        crank_talk_StringStream_insertCString(context, stream, "Anonymous Class");
    else
        crank_talk_StringStream_insertBytesObject(context, stream, (crank_talk_oop_t)clazz->name);
    return receiver;
}

static crank_talk_oop_t
crank_talk_Metaclass_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_Metaclass_t *metaclass = (crank_talk_Metaclass_t*)receiver;
    if(!metaclass->thisClass)
    {
        crank_talk_StringStream_insertCString(context, stream, "Anonymous Metaclass");
    }
    else
    {
        crank_talk_StringStream_print(context, stream, (crank_talk_oop_t)metaclass->thisClass);
        crank_talk_StringStream_insertCString(context, stream, " class");
    }
    return receiver;
}

static crank_talk_oop_t
crank_talk_UndefinedObject_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCString(context, stream, "nil");
    return receiver;
}

static crank_talk_oop_t
crank_talk_False_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCString(context, stream, "false");
    return receiver;
}

static crank_talk_oop_t
crank_talk_True_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCString(context, stream, "true");
    return receiver;
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
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCharacter(context, stream, '\'');
    crank_talk_StringStream_insertBytesObject(context, stream, receiver);
    crank_talk_StringStream_insertCharacter(context, stream, '\'');
    return receiver;
}

static crank_talk_oop_t
crank_talk_String_concat_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t left = arguments[0];
    crank_talk_oop_t right = arguments[1];
    return crank_talk_String_concat(context, left, right);
}

static crank_talk_oop_t
crank_talk_String_withoutElementAtIndex_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t string = arguments[0];
    intptr_t index = crank_talk_oop_decodeSmallInteger(arguments[1]);
    return crank_talk_String_withoutElementAtIndex(context, string, index - 1);
}

static crank_talk_oop_t
crank_talk_String_insertStringAtIndex_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t string = arguments[0];
    crank_talk_oop_t insertedString = arguments[1];
    intptr_t index = crank_talk_oop_decodeSmallInteger(arguments[2]);
    return crank_talk_String_insertStringAtIndex(context, string, insertedString, index - 1);
}



static crank_talk_oop_t
crank_talk_Symbol_asString_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_oop_t receiver = arguments[0];
    return crank_talk_String_createWithString(context, crank_talk_object_getSizeInBytes(receiver), (const char*)crank_talk_oop_getBytesData(receiver));
}

static crank_talk_oop_t
crank_talk_Symbol_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_StringStream_insertCharacter(context, stream, '#');
    crank_talk_StringStream_insertBytesObject(context, stream, receiver);
    return receiver;
}

static crank_talk_oop_t
crank_talk_StringStream_nextPut_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t stream = arguments[0];
    crank_talk_oop_t character = arguments[1];
    if(!crank_talk_oop_isCharacter(character))
        crank_talk_errorPrintf(context, "Expected a character for nextPut:.");
    crank_talk_StringStream_insertCharacter(context, stream, crank_talk_oop_decodeCharacter(character));
    return stream;
}

static crank_talk_oop_t
crank_talk_StringStream_nextPutAll_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t stream = arguments[0];
    crank_talk_oop_t bytes = arguments[1];
    crank_talk_StringStream_insertBytesObject(context, stream, bytes);
    return stream;
}

static crank_talk_oop_t
crank_talk_StringStream_print_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t stream = arguments[0];
    crank_talk_oop_t object = arguments[1];
    crank_talk_StringStream_print(context, stream, object);
    return stream;
}

static crank_talk_oop_t
crank_talk_StringStream_space_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);
    crank_talk_oop_t stream = arguments[0];
    crank_talk_StringStream_insertCharacter(context, stream, ' ');
    return stream;
}

static crank_talk_oop_t
crank_talk_StringStream_lf_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);
    crank_talk_oop_t stream = arguments[0];
    crank_talk_StringStream_insertCharacter(context, stream, '\n');
    return stream;
}

void crank_talk_registerStringPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "asString", 1, crank_talk_Object_asString_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "printString", 1, crank_talk_Object_printString_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "printOn:", 2, crank_talk_Object_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "fullPrintOn:", 2, crank_talk_Object_fullPrintOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ClassClass, "printOn:", 2, crank_talk_Class_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.MetaclassClass, "printOn:", 2, crank_talk_Metaclass_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.UndefinedObjectClass, "printOn:", 2, crank_talk_UndefinedObject_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "printOn:", 2, crank_talk_False_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "printOn:", 2, crank_talk_True_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringClass, "asString", 1, crank_talk_String_asString_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringClass, "printOn:", 2, crank_talk_String_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SymbolClass, "asString", 1, crank_talk_Symbol_asString_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SymbolClass, "printOn:", 2, crank_talk_Symbol_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringClass, ",", 2, crank_talk_String_concat_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringClass, "withoutElementAtIndex:", 2, crank_talk_String_withoutElementAtIndex_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringClass, "insertString:atIndex:", 3, crank_talk_String_insertStringAtIndex_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringStreamClass, "nextPut:", 2, crank_talk_StringStream_nextPut_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringStreamClass, "nextPutAll:", 2, crank_talk_StringStream_nextPutAll_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringStreamClass, "print:", 2, crank_talk_StringStream_print_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringStreamClass, "space", 1, crank_talk_StringStream_space_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.StringStreamClass, "lf", 1, crank_talk_StringStream_lf_primitive);

}