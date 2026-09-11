#ifndef SYSMEL_VM_BYTES_H
#define SYSMEL_VM_BYTES_H

#include "object-model.h"

// Byte array
CRANK_TALK_API crank_talk_oop_t crank_talk_ByteArray_create(crank_talk_VMContext_t *context, size_t size);

// Byte stream
CRANK_TALK_API crank_talk_oop_t crank_talk_ByteStream_create(crank_talk_VMContext_t *context);
CRANK_TALK_API void crank_talk_ByteStream_atPut(crank_talk_oop_t stream, size_t position, uint8_t byte);

CRANK_TALK_API void crank_talk_ByteStream_insertByte(crank_talk_VMContext_t *context, crank_talk_oop_t stream, uint8_t bytes);
CRANK_TALK_API void crank_talk_ByteStream_insertBytes(crank_talk_VMContext_t *context, crank_talk_oop_t stream, size_t size, uint8_t *bytes);
CRANK_TALK_API void crank_talk_ByteStream_insertBytesObject(crank_talk_VMContext_t *context, crank_talk_oop_t stream, crank_talk_oop_t object);

CRANK_TALK_API size_t crank_talk_ByteStream_getSize(crank_talk_oop_t stream);

CRANK_TALK_API crank_talk_oop_t crank_talk_ByteStream_asByteArray(crank_talk_VMContext_t *context, crank_talk_oop_t stream);

#endif //SYSMEL_VM_BYTES_H