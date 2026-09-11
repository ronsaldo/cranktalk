#include "crank-talk/bytes.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

CRANK_TALK_API crank_talk_oop_t
crank_talk_ByteArray_create(crank_talk_VMContext_t *context, size_t size)
{
    return CrankAllocateVariadicObjectWithClassName(context, ByteArray, CrankTalkGCObjectDataTypeBytes, size);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_ByteStream_create(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t streamValue = CrankAllocateValuesObjectWithClassName(context, ByteStream);
    crank_talk_ByteStream_t *stream = (crank_talk_ByteStream_t*)streamValue;
    stream->size = crank_talk_oop_encodeSmallInteger(0);
    return streamValue;
}

static void
crank_talk_ByteStream_increaseCapacity(crank_talk_VMContext_t *context, crank_talk_ByteStream_t *stream)
{
    size_t size = crank_talk_oop_decodeSmallInteger(stream->size);
    size_t capacity = crank_talk_object_getSizeInBytes((crank_talk_oop_t)stream->storage);
    size_t newCapacity = capacity*2;
    if(newCapacity < 16)
        newCapacity = 16;

    crank_talk_oop_t oldStorage = (crank_talk_oop_t)stream->storage;
    crank_talk_oop_t newStorage = CrankAllocateVariadicObjectWithClassName(context, ByteArray, CrankTalkGCObjectDataTypeBytes, newCapacity);
    memcpy(crank_talk_oop_getBytesData(newStorage), crank_talk_oop_getBytesData(oldStorage), size);
    stream->storage = (crank_talk_ByteArray_t*)newStorage;
}


CRANK_TALK_API void
crank_talk_ByteStream_atPut(crank_talk_oop_t stream, size_t position, uint8_t byte)
{
    crank_talk_ByteStream_t *stringStream = (crank_talk_ByteStream_t*)stream;
    size_t size = crank_talk_oop_decodeSmallInteger(stringStream->size);
    if(position < 1 || position >= size)
        crank_talk_error("Byte stream index is out of bounds");

    crank_talk_ByteArray_t *storage = (crank_talk_ByteArray_t*)stringStream->storage;
    storage->elements[position - 1] = byte;
}

CRANK_TALK_API void
crank_talk_ByteStream_insertByte(crank_talk_VMContext_t *context, crank_talk_oop_t stream, uint8_t byte)
{
    crank_talk_ByteStream_t *stringStream = (crank_talk_ByteStream_t*)stream;
    size_t size = crank_talk_oop_decodeSmallInteger(stringStream->size);
    size_t capacity = crank_talk_object_getSizeInBytes((crank_talk_oop_t)stringStream->storage);
    if(size >= capacity)
        crank_talk_ByteStream_increaseCapacity(context, stringStream);

    crank_talk_ByteArray_t *storage = (crank_talk_ByteArray_t*)stringStream->storage;
    storage->elements[size] = byte;
    stringStream->size = crank_talk_oop_encodeSmallInteger(size + 1);
}

CRANK_TALK_API void crank_talk_ByteStream_insertBytes(crank_talk_VMContext_t *context, crank_talk_oop_t stream, size_t size, uint8_t *bytes)
{
    for(size_t i = 0; i < size; ++i)
        crank_talk_ByteStream_insertByte(context, stream, bytes[i]);
}

CRANK_TALK_API void
crank_talk_ByteStream_insertBytesObject(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t object)
{
    size_t size = crank_talk_object_getSizeInBytes(object);
    crank_talk_ByteStream_insertBytes(context, stream, size, crank_talk_oop_getBytesData(object));
}

CRANK_TALK_API size_t
crank_talk_ByteStream_getSize(crank_talk_oop_t stream)
{
    crank_talk_ByteStream_t *byteStream = (crank_talk_ByteStream_t*)stream;
    return crank_talk_oop_decodeSmallInteger(byteStream->size);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_ByteStream_asByteArray(crank_talk_VMContext_t *context, crank_talk_oop_t stream)
{
    crank_talk_ByteStream_t *byteStream = (crank_talk_ByteStream_t*)stream;
    size_t byteCount = crank_talk_oop_decodeSmallInteger(byteStream->size);
    crank_talk_oop_t byteArray = crank_talk_ByteArray_create(context, byteCount);
    memcpy(crank_talk_oop_getBytesData(byteArray), byteStream->storage->elements, byteCount);
    return byteArray;
}