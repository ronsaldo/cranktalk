#ifndef CRANK_TALK_DYNARRAY_CONTEXT_H
#define CRANK_TALK_DYNARRAY_CONTEXT_H

#include <stddef.h>
#include <stdint.h>
#include "common.h"

typedef struct crank_talk_dynarray_s
{
    size_t entrySize;
    size_t size;
    size_t capacity;
    uint8_t *data;
} crank_talk_dynarray_t;

CRANK_TALK_API void crank_talk_dynarray_initialize(crank_talk_dynarray_t *dynarray, size_t entrySize, size_t initialCapacity);
CRANK_TALK_API size_t crank_talk_dynarray_addAll(crank_talk_dynarray_t *dynarray, size_t entryCount, const void *newEntries);
CRANK_TALK_API size_t crank_talk_dynarray_add(crank_talk_dynarray_t *dynarray, const void *newEntry);
CRANK_TALK_API void crank_talk_dynarray_destroy(crank_talk_dynarray_t *dynarray);

#define crank_talk_dynarray_entryOfTypeAt(dynarray, entryType, index) (((entryType*)(dynarray).data) + index)

#endif //SYSBVM_DYNARRAY_CONTEXT_H