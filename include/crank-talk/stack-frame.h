#ifndef CRANK_TALK_STACK_FRAME_H
#define CRANK_TALK_STACK_FRAME_H

#include "object-model.h"

#define CRANK_TAL_MAX_CALL_ARGUMENT_COUNT 16

typedef void (crank_talk_GCRootIterationFunction_t) (void *userdata, crank_talk_oop_t *root);

typedef enum sysbvm_stackFrameRecordType_e
{
    CrankTalkStackFrameRecordGCRoots = 0,
    CrankTalkStackFrameRecordApplicationArguments,
    CrankTalkStackFrameRecordBytecodeFunctionActivationRecord,
    CrankTalkStackFrameRecordCount,
} crank_talk_stackFrameRecordType_t;

typedef struct crank_talk_stackFrameRecord_s
{
    struct crank_talk_stackFrameRecord_s *previous;
    crank_talk_stackFrameRecordType_t type;
} crank_talk_stackFrameRecord_t;

typedef struct crank_talk_stackFrameGCRootsRecord_s
{
    crank_talk_stackFrameRecord_t super;
    size_t rootCount;
    crank_talk_oop_t *roots;
} crank_talk_stackFrameGCRootsRecord_t;

typedef struct crank_talk_stackFrameApplicationArgumentsRecord_s
{
    crank_talk_stackFrameRecord_t super;
    crank_talk_oop_t functional;
    crank_talk_oop_t blockClosure;
    size_t argumentCount;
    crank_talk_oop_t *arguments;
} crank_talk_stackFrameApplicationArgumentsRecord_t;

#define SYSMEL_VM_STACKFRAME_PUSH_GC_ROOTS(recordName, gcStackFrameRoots) \
    crank_talk_stackFrameGCRootsRecord_t recordName = {0}; \
    recordName.super.type = CrankTalkStackFrameRecordGCRoots; \
    recordName.rootCount = sizeof(gcStackFrameRoots) / sizeof(crank_talk_Value_t); \
    recordName.roots = (crank_talk_Value_t*)(&gcStackFrameRoots); \
    crank_talk_stackFrame_pushRecord(&recordName.super)

#define SYSMEL_VM_STACKFRAME_POP_GC_ROOTS(recordName) \
    crank_talk_stackFrame_popRecord(&recordName.super)

CRANK_TALK_API crank_talk_stackFrameRecord_t *crank_talk_stackFrame_getActiveRecord(void);

CRANK_TALK_API void crank_talk_stackFrame_pushRecord(crank_talk_stackFrameRecord_t *record);
CRANK_TALK_API void crank_talk_stackFrame_popRecord(crank_talk_stackFrameRecord_t *record);

CRANK_TALK_API void crank_talk_stackFrame_iterateGCRootsInRecordWith(crank_talk_stackFrameRecord_t *record, void *userdata, crank_talk_GCRootIterationFunction_t iterationFunction);
CRANK_TALK_API void crank_talk_stackFrame_iterateGCRootsInStackWith(crank_talk_stackFrameRecord_t *stackBottomRecord, void *userdata, crank_talk_GCRootIterationFunction_t iterationFunction);


#endif