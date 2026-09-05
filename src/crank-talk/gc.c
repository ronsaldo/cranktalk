#include "crank-talk/gc.h"
#include "crank-talk/context.h"
#include <stdlib.h>

static void
crank_talk_gc_markPointer(crank_talk_VMContext_t *context, crank_talk_ObjectHeader_t *pointer)
{
    if(!pointer)
        return;

    bool isWhite = pointer->gcColor == context->heap.gcWhiteColor;
    if(!isWhite)
        return;

    pointer->gcColor = context->heap.gcGrayColor;
    crank_talk_dynarray_add(&context->markingStack, &pointer);
}

static void
crank_talk_gc_markOop(void *userdata, crank_talk_oop_t *valuePointer)
{
    if(!crank_talk_oop_isNonNullPointer(*valuePointer))
        return;

    crank_talk_gc_markPointer((crank_talk_VMContext_t*)userdata, (crank_talk_ObjectHeader_t*)*valuePointer);
}

static void
crank_talk_gc_markObjectContent(crank_talk_VMContext_t *context, crank_talk_ObjectHeader_t *objectPointer)
{
    crank_talk_gc_markPointer(context, (crank_talk_ObjectHeader_t*)objectPointer->behavior);
    assert(objectPointer->byteSize >= sizeof(crank_talk_ObjectHeader_t));

    if(objectPointer->dataType == CrankTalkGCObjectDataTypeOops)
    {
        size_t byteSize = objectPointer->byteSize - sizeof(crank_talk_ObjectHeader_t);
        size_t valueCount = byteSize / sizeof(crank_talk_oop_t);
        crank_talk_oop_t *objectValues = (crank_talk_oop_t*)(objectPointer + 1);
        for(size_t i = 0; i < valueCount; ++i)
            crank_talk_gc_markOop(context, objectValues + i);
    }
    objectPointer->gcColor = context->heap.gcBlackColor;
}

static void
crank_talk_gc_markUntilStackIsEmpty(crank_talk_VMContext_t *context)
{
    while(context->markingStack.size > 0)
    {
        crank_talk_ObjectHeader_t **pendingObjects = (crank_talk_ObjectHeader_t**)context->markingStack.data;
        crank_talk_ObjectHeader_t *nextPendingObject = pendingObjects[--context->markingStack.size];
        crank_talk_gc_markObjectContent(context, nextPendingObject);
    }
}

CRANK_TALK_API void
crank_talk_gc_collect(crank_talk_VMContext_t *context)
{
    context->heap.shouldAttemptToCollect = true;
    crank_talk_gc_safepoint(context);
}

void
crank_talk_gc_performCycle(crank_talk_VMContext_t *context)
{
    crank_talk_gc_iterateRoots(context, context, crank_talk_gc_markOop);
}

CRANK_TALK_API void
crank_talk_gc_safepoint(crank_talk_VMContext_t *context)
{
    if(!context->heap.shouldAttemptToCollect || context->gcDisabled)
        return;

    // Phase 1: marking
    crank_talk_gc_performCycle(context);
    crank_talk_gc_markUntilStackIsEmpty(context);

    // Phase 2: replace weak references with tombstones.
    // TODO:

    // Phase 3: Sweep
    crank_talk_heap_sweep(&context->heap);

    // Phase 4: Swap th GC colors.
    crank_talk_heap_swapGCColors(&context->heap);
}

CRANK_TALK_API void
crank_talk_gc_iterateRoots(crank_talk_VMContext_t *context, void *userdata, crank_talk_GCRootIterationFunction_t iterationFunction)
{
    // Context roots.
    crank_talk_oop_t *contextRoots = (crank_talk_oop_t*)&context->roots;
    size_t contextRootCount = sizeof(context->roots) / sizeof(crank_talk_oop_t);
    for(size_t i = 0; i < contextRootCount; ++i)
        iterationFunction(userdata, contextRoots + i);

    // Stack roots
    crank_talk_stackFrame_iterateGCRootsInStackWith(crank_talk_stackFrame_getActiveRecord(), userdata, iterationFunction);
}