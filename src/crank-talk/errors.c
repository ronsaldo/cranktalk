#include "crank-talk/errors.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include "crank-talk/source-position.h"
#include "crank-talk/stack-frame.h"
#include "crank-talk/string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

crank_talk_stackFrameOnDoRecord_t *
crank_talk_StackFrame_findHandlerForType(crank_talk_VMContext_t *context, crank_talk_oop_t exceptionType)
{
    crank_talk_stackFrameRecord_t *currentRecord = crank_talk_stackFrame_getActiveRecord();
    while(currentRecord)
    {
        if(currentRecord->type == CrankTalkStackFrameRecordOnDoRecord)
        {
            crank_talk_stackFrameOnDoRecord_t *onDoRecord = (crank_talk_stackFrameOnDoRecord_t*)currentRecord;
            if(sysmel_vm_Behavior_isDirectSubtype(exceptionType, onDoRecord->exceptionClass))
                return onDoRecord;
        }

        currentRecord = currentRecord->previous;
    }

    return NULL;
}

CRANK_TALK_API void
crank_talk_Exception_signal(crank_talk_VMContext_t *context, crank_talk_Exception_t *exception)
{
    crank_talk_stackFrameOnDoRecord_t *handlingContext = crank_talk_StackFrame_findHandlerForType(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, (crank_talk_oop_t)exception));
    if(handlingContext)
    {
        crank_talk_oop_t doResult = crank_talk_send0(context, context->roots.valueSymbol, handlingContext->exceptionAction);
        handlingContext->result = doResult;
        crank_talk_stackFrame_unwindAndJumpToOnDoBlock(context, handlingContext);
        abort();
    }

    printf("UnhandledException: " CRANK_TALK_STRING_FORMAT"\n", CRANK_TALK_STRING_FORMAT_ARG(exception->messageText));
    crank_talk_stackFrame_printStackTrace(context);
    abort();
}

CRANK_TALK_API void
crank_talk_error(crank_talk_VMContext_t *context, const char *message)
{
    crank_talk_Error_t *error = (crank_talk_Error_t *)CrankAllocateValuesObjectWithClassName(context, Error);
    error->super.messageText = crank_talk_String_createWithCString(context, message);
    crank_talk_Exception_signal(context, &error->super);
    abort();
}

CRANK_TALK_API void
crank_talk_errorPrintf(crank_talk_VMContext_t *context, const char *format, ...)
{
    (void)context;
    char buffer[256];

    va_list valist;
    va_start(valist, format);
    vsnprintf(buffer, sizeof(buffer), format, valist);
    va_end(valist);

    crank_talk_error(context, buffer);
}

CRANK_TALK_API void
crank_talk_errorAtSourcePosition(crank_talk_VMContext_t *context, crank_talk_oop_t abstractSourcePosition, const char *message)
{
    if(crank_talk_oop_isKindOf(context, abstractSourcePosition, context->roots.SourcePositionClass))
    {
        crank_talk_SourcePosition_t *sourcePosition = (crank_talk_SourcePosition_t*)abstractSourcePosition;
        crank_talk_SourceCode_t *sourceCode = (crank_talk_SourceCode_t *)sourcePosition->sourceCode;
        fprintf(stderr, CRANK_TALK_STRING_FORMAT "/" CRANK_TALK_STRING_FORMAT ":%d.%d-%d.%d: ",
            CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)sourceCode->directory),
            CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)sourceCode->name),
            (int)crank_talk_oop_decodeSmallInteger(sourcePosition->startLine), (int)crank_talk_oop_decodeSmallInteger(sourcePosition->startColumn),
            (int)crank_talk_oop_decodeSmallInteger(sourcePosition->endLine), (int)crank_talk_oop_decodeSmallInteger(sourcePosition->endColumn));

    }

    fprintf(stderr, "Error: %s\n", message);
    abort();
}

CRANK_TALK_API void
crank_talk_errorPrintfAtSourcePosition(crank_talk_VMContext_t *context, crank_talk_oop_t sourcePosition, const char *format, ...)
{
    char buffer[256];

    va_list valist;
    va_start(valist, format);
    vsnprintf(buffer, sizeof(buffer), format, valist);
    va_end(valist);

    crank_talk_errorAtSourcePosition(context, sourcePosition, buffer);
}

static crank_talk_oop_t
crank_talk_BlockClosure_onDo_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 3);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t exceptionClass = arguments[1];
    crank_talk_oop_t exceptionAction = arguments[2];

    if(!crank_talk_oop_isKindOf(context, exceptionClass, context->roots.BehaviorClass))
        crank_talk_error(context, "Expected a behavior for on:do:");

    if(!crank_talk_oop_isKindOf(context, exceptionAction, context->roots.BlockClosureClass))
        crank_talk_error(context, "Expected a block closure for on:do:");

    crank_talk_stackFrameOnDoRecord_t onDoRecord = {
        .super.type = CrankTalkStackFrameRecordOnDoRecord,
        .exceptionClass = exceptionClass,
        .exceptionAction = exceptionAction,
        .result = crank_talk_oop_nil()
    };

    crank_talk_stackFrame_pushRecord(&onDoRecord.super);

    if(!_setjmp(onDoRecord.jmpbuffer))
        onDoRecord.result = crank_talk_send0(context, context->roots.valueSymbol, self);

    crank_talk_stackFrame_popRecord(&onDoRecord.super);
    return onDoRecord.result;
}

static crank_talk_oop_t
crank_talk_BlockClosure_ensure_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t ensureBlock = arguments[1];

    crank_talk_stackFrameEnsureRecord_t ensureRecord = {
        .super.type = CrankTalkStackFrameRecordEnsureRecord,
        .ensureBlock = ensureBlock,
    };

    crank_talk_stackFrame_pushRecord(&ensureRecord.super);

    crank_talk_oop_t result = crank_talk_send0(context, context->roots.valueSymbol, self);

    crank_talk_stackFrame_popRecord(&ensureRecord.super);
    return result;
}

static crank_talk_oop_t
crank_talk_Exception_signal_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_Exception_t *exception = (crank_talk_Exception_t *)arguments[0];
    crank_talk_Exception_signal(context, exception);
    return exception->resumeValue;
}

static crank_talk_oop_t
crank_talk_Exception_signalWith_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_Exception_t *exception = (crank_talk_Exception_t *)arguments[0];
    exception->messageText = arguments[1];

    crank_talk_Exception_signal(context, exception);
    return exception->resumeValue;
}

void
crank_talk_registerErrorPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "on:do:", 3, crank_talk_BlockClosure_onDo_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "ensure:", 2, crank_talk_BlockClosure_ensure_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ExceptionClass, "signal", 1, crank_talk_Exception_signal_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ExceptionClass, "signal:", 2, crank_talk_Exception_signalWith_primitive);
}