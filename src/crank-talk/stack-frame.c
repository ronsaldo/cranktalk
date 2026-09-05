#include "crank-talk/stack-frame.h"
#include <assert.h>
#include <stdlib.h>

CRANK_TALK_THREAD_LOCAL crank_talk_stackFrameRecord_t *
crank_talk_stackFrame_activeRecord;

CRANK_TALK_API crank_talk_stackFrameRecord_t*
crank_talk_stackFrame_getActiveRecord(void)
{
    return crank_talk_stackFrame_activeRecord;
}

CRANK_TALK_API void
crank_talk_stackFrame_pushRecord(crank_talk_stackFrameRecord_t *record)
{
    assert(record->type < CrankTalkStackFrameRecordCount);
    record->previous = crank_talk_stackFrame_activeRecord;
    crank_talk_stackFrame_activeRecord = record;
}

CRANK_TALK_API void
crank_talk_stackFrame_popRecord(crank_talk_stackFrameRecord_t *record)
{
    assert(record->type < CrankTalkStackFrameRecordCount);
    crank_talk_stackFrame_activeRecord = record->previous;
    assert(!crank_talk_stackFrame_activeRecord || crank_talk_stackFrame_activeRecord->type < CrankTalkStackFrameRecordCount);
}

CRANK_TALK_API void
crank_talk_stackFrame_iterateGCRootsInRecordWith(crank_talk_stackFrameRecord_t *record, void *userdata, crank_talk_GCRootIterationFunction_t iterationFunction)
{
    switch(record->type)
    {
    case CrankTalkStackFrameRecordGCRoots:
    {
        crank_talk_stackFrameGCRootsRecord_t *rootsRecord = (crank_talk_stackFrameGCRootsRecord_t*)record;

        // Oop roots.
        for(size_t i = 0; i < rootsRecord->rootCount; ++i)
        {
            iterationFunction(userdata, &rootsRecord->roots[i]);
        }
    } break;
    default: abort();
    }
}

CRANK_TALK_API void
crank_talk_stackFrame_iterateGCRootsInStackWith(crank_talk_stackFrameRecord_t *stackBottomRecord, void *userdata, crank_talk_GCRootIterationFunction_t iterationFunction)
{
    for(crank_talk_stackFrameRecord_t *record = stackBottomRecord; record; record = record->previous)
        crank_talk_stackFrame_iterateGCRootsInRecordWith(record, userdata, iterationFunction);
}