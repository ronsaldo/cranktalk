#include "crank-talk/heap.h"
#include <stdlib.h>

#define CRANK_TALK_HEAP_MIN_CHUNK_SIZE (2<<20)
#define CRANK_TALK_HEAP_STARTUP_HEAP_SIZE (CRANK_TALK_HEAP_MIN_CHUNK_SIZE*4)
#define CRANK_TALK_HEAP_COLLECTION_GAMMA_FACTOR 3

CRANK_TALK_API void
crank_talk_heap_initialize(crank_talk_heap_t *heap)
{
    heap->identityHashSeed = 42;
    
    heap->gcWhiteColor = 0;
    heap->gcGrayColor = 1;
    heap->gcBlackColor = 2;
}

static void
crank_talk_heap_checkForGCThreshold(crank_talk_heap_t *heap)
{
    if(heap->shouldAttemptToCollect)
        return;

    heap->shouldAttemptToCollect = heap->totalSize > heap->nextGCSizeThreshold;
}

CRANK_TALK_API crank_talk_ObjectHeader_t *
crank_talk_heap_allocateObject(crank_talk_heap_t *heap, size_t objectSize)
{
    size_t allocationSize = sizeof(crank_talk_ObjectHeader_t) + objectSize;
    crank_talk_ObjectHeader_t *objectHeader = calloc(1, allocationSize);
    objectHeader->byteSize = objectSize;
    objectHeader->identityHash = heap->identityHashSeed;
    objectHeader->gcColor = heap->gcWhiteColor;
    heap->identityHashSeed = crank_talk_hashMultiply(heap->identityHashSeed);

    // Count the allocated data
    heap->totalSize += objectSize;

    // Store the object on the heap.
    if(heap->lastObject)
    {
        heap->lastObject->nextObject = objectHeader;
        heap->lastObject = objectHeader;
    }
    else
    {
        heap->firstObject = heap->lastObject = objectHeader;
    }

    crank_talk_heap_checkForGCThreshold(heap);
    return objectHeader;
}

CRANK_TALK_API crank_talk_ObjectHeader_t *
crank_talk_heap_allocateObjectWithSizeAndType(crank_talk_heap_t *heap, size_t objectSize, crank_talk_Behavior_t *behavior)
{
    crank_talk_ObjectHeader_t *object = crank_talk_heap_allocateObject(heap, objectSize);
    object->behavior = behavior;
    return object;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_heap_allocateOopWithSizeAndType(crank_talk_heap_t *heap, size_t objectSize, crank_talk_Behavior_t *behavior)
{
    return (crank_talk_oop_t)crank_talk_heap_allocateObjectWithSizeAndType(heap, objectSize, behavior);
}

static void
sysbvm_heap_computeNextCollectionThreshold(crank_talk_heap_t *heap)
{
    heap->shouldAttemptToCollect = false;

    size_t liveDataSize = heap->totalSize;
    if(liveDataSize < CRANK_TALK_HEAP_STARTUP_HEAP_SIZE)
        liveDataSize = CRANK_TALK_HEAP_STARTUP_HEAP_SIZE;
    heap->nextGCSizeThreshold = liveDataSize * CRANK_TALK_HEAP_COLLECTION_GAMMA_FACTOR;
}

CRANK_TALK_API void
crank_talk_heap_sweep(crank_talk_heap_t *heap)
{
    crank_talk_ObjectHeader_t *position = heap->firstObject;
    heap->firstObject = heap->lastObject = NULL;

    while(position)
    {
        crank_talk_ObjectHeader_t *next = position->nextObject;
        position->nextObject = NULL;

        if(position->gcColor == heap->gcBlackColor)
        {
            // Add the object back to the list.
            if(heap->firstObject)
            {
                heap->lastObject->nextObject = position;
                heap->lastObject = position;
            }
            else
            {
                heap->firstObject = heap->lastObject = position;
            }
        }
        else
        {
            assert(heap->totalSize >= position->byteSize);
            heap->totalSize -= position->byteSize;
            free(position);
        }

        position = next;
    }

    sysbvm_heap_computeNextCollectionThreshold(heap);
}

CRANK_TALK_API void
crank_talk_heap_swapGCColors(crank_talk_heap_t *heap)
{
    uint32_t temp = heap->gcBlackColor;
    heap->gcBlackColor = heap->gcWhiteColor;
    heap->gcWhiteColor = temp;
}

CRANK_TALK_API void
crank_talk_heap_freeAll(crank_talk_heap_t *heap)
{
    crank_talk_ObjectHeader_t *position = heap->firstObject;
    while(position)
    {
        crank_talk_ObjectHeader_t *toDelete = position;
        position = position->nextObject;
        free(toDelete);
    }
}