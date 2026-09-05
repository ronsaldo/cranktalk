#ifndef CRANK_TALK_HEAP_H
#define CRANK_TALK_HEAP_H

#pragma once

#include "object-model.h"

typedef struct crank_talk_heap_s
{
    crank_talk_ObjectHeader_t *firstObject;
    crank_talk_ObjectHeader_t *lastObject;

    size_t identityHashSeed;
        
    size_t totalSize;
    size_t totalCapacity;
    size_t nextGCSizeThreshold;

    bool shouldAttemptToCollect;

    uint32_t gcWhiteColor;
    uint32_t gcGrayColor;
    uint32_t gcBlackColor;
} crank_talk_heap_t;

CRANK_TALK_API void crank_talk_heap_initialize(crank_talk_heap_t *heap);

CRANK_TALK_API crank_talk_ObjectHeader_t *crank_talk_heap_allocateObject(crank_talk_heap_t *heap, size_t objectSize);
CRANK_TALK_API crank_talk_ObjectHeader_t *crank_talk_heap_allocateObjectWithSizeAndType(crank_talk_heap_t *heap, size_t objectSize, crank_talk_Behavior_t *behavior);
CRANK_TALK_API crank_talk_oop_t crank_talk_heap_allocateOopWithSizeAndType(crank_talk_heap_t *heap, size_t objectSize, crank_talk_Behavior_t *behavior);

CRANK_TALK_API void crank_talk_heap_sweep(crank_talk_heap_t *heap);
CRANK_TALK_API void crank_talk_heap_swapGCColors(crank_talk_heap_t *heap);
CRANK_TALK_API void crank_talk_heap_freeAll(crank_talk_heap_t *heap);

#endif //CRANK_TALK_HEAP_H

