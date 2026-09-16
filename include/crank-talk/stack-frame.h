#ifndef CRANK_TALK_STACK_FRAME_H
#define CRANK_TALK_STACK_FRAME_H

#include "object-model.h"
#include <setjmp.h>

#ifdef _WIN32
#define _longjmp longjmp
#define _setjmp setjmp
#endif

#define CRANK_TALK_MAX_CALL_ARGUMENT_COUNT 16

typedef void (crank_talk_GCRootIterationFunction_t) (void *userdata, crank_talk_oop_t *root);

typedef enum sysbvm_stackFrameRecordType_e
{
    CrankTalkStackFrameRecordGCRoots = 0,
    CrankTalkStackFrameRecordApplicationArguments,
    CrankTalkStackFrameRecordBytecodeFunctionActivationRecord,
    CrankTalkStackFrameRecordEnsureRecord,
    CrankTalkStackFrameRecordOnDoRecord,
    CrankTalkStackFrameRecordCount,
} crank_talk_stackFrameRecordType_t;

typedef struct crank_talk_stackFrameRecord_s
{
    struct crank_talk_stackFrameRecord_s *previous;
    crank_talk_stackFrameRecordType_t type;
    crank_talk_oop_t mappedContext;
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

typedef struct crank_talk_stackFrameBytecodeFunctionActivationRecord_s
{
    crank_talk_stackFrameRecord_t super;
    crank_talk_oop_t function;
    crank_talk_oop_t bytecode;

    crank_talk_oop_t blockClosureOrNil;
    crank_talk_oop_t outerContext;
    crank_talk_oop_t captureVector;
    crank_talk_oop_t literalVector;

    size_t argumentCount;
    crank_talk_oop_t *arguments;

    size_t temporaryCount;
    crank_talk_oop_t *temporaries;

    size_t calloutArgumentCount;
    crank_talk_oop_t calloutArguments[CRANK_TALK_MAX_CALL_ARGUMENT_COUNT];

    crank_talk_oop_t result;

    crank_talk_Array_t *arrayInConstruction;
    size_t arrayInConstructionSize;
    crank_talk_oop_t closureInConstruction;

    size_t pc;
    jmp_buf jmpbuffer;
} crank_talk_stackFrameBytecodeFunctionActivationRecord_t;

typedef struct crank_talk_stackFrameOnDoRecord_s
{
    crank_talk_stackFrameRecord_t super;
    crank_talk_oop_t exceptionClass;
    crank_talk_oop_t exceptionAction;
    crank_talk_oop_t result;
    jmp_buf jmpbuffer;
} crank_talk_stackFrameOnDoRecord_t;

typedef struct crank_talk_stackFrameEnsureRecord_s
{
    crank_talk_stackFrameRecord_t super;
    crank_talk_oop_t ensureBlock;
} crank_talk_stackFrameEnsureRecord_t;

#define CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(recordName, gcStackFrameRoots) \
    crank_talk_stackFrameGCRootsRecord_t recordName = {0}; \
    recordName.super.type = CrankTalkStackFrameRecordGCRoots; \
    recordName.rootCount = sizeof(gcStackFrameRoots) / sizeof(crank_talk_oop_t); \
    recordName.roots = (crank_talk_oop_t*)(&gcStackFrameRoots); \
    crank_talk_stackFrame_pushRecord(&recordName.super)

#define CRANK_TALK_STACKFRAME_POP_GC_ROOTS(recordName) \
    crank_talk_stackFrame_popRecord(&recordName.super)

CRANK_TALK_API crank_talk_stackFrameRecord_t *crank_talk_stackFrame_getActiveRecord(void);

CRANK_TALK_API void crank_talk_stackFrame_pushRecord(crank_talk_stackFrameRecord_t *record);
CRANK_TALK_API void crank_talk_stackFrame_popRecord(crank_talk_stackFrameRecord_t *record);
CRANK_TALK_API void crank_talk_stackFrame_unwindPopRecord(crank_talk_VMContext_t *context, crank_talk_stackFrameRecord_t *record);

CRANK_TALK_API void crank_talk_stackFrame_unwindAndJumpToBytecodeMethod(crank_talk_VMContext_t *context, crank_talk_stackFrameBytecodeFunctionActivationRecord_t *targetRecord);
CRANK_TALK_API void crank_talk_stackFrame_unwindAndJumpToOnDoBlock(crank_talk_VMContext_t *context, crank_talk_stackFrameOnDoRecord_t *targetRecord);

CRANK_TALK_API void crank_talk_stackFrame_iterateGCRootsInRecordWith(crank_talk_stackFrameRecord_t *record, void *userdata, crank_talk_GCRootIterationFunction_t iterationFunction);
CRANK_TALK_API void crank_talk_stackFrame_iterateGCRootsInStackWith(crank_talk_stackFrameRecord_t *stackBottomRecord, void *userdata, crank_talk_GCRootIterationFunction_t iterationFunction);

CRANK_TALK_API void crank_talk_stackFrame_printStackTrace(crank_talk_VMContext_t *context);

#endif