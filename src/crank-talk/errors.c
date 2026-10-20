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
    (void)context;
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
        crank_talk_BlockClosure_t *actionBlock = (crank_talk_BlockClosure_t *)handlingContext->exceptionAction;
        size_t actionBlockArgumentCount = crank_talk_oop_decodeSmallInteger(actionBlock->function->argumentCount);
        if(actionBlockArgumentCount == 0)
        {
            crank_talk_oop_t doResult = crank_talk_send0(context, context->roots.valueSymbol, handlingContext->exceptionAction);
            handlingContext->result = doResult;
        }
        else if(actionBlockArgumentCount == 1)
        {
            crank_talk_oop_t doResult = crank_talk_send1(context, context->roots.valueWithSymbol, handlingContext->exceptionAction, (crank_talk_oop_t)exception);
            handlingContext->result = doResult;
        }
        else
        {
            fprintf(stderr, "Expected a block closure with at most one argument for on:do:.");
            abort();
        }
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

static void
crank_talk_Object_assert_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t assertValue = primContext->arguments[1];
    if(assertValue != primContext->context->roots.trueValue)
    {        
        crank_talk_AssertionFailure_t *assertionFailure = (crank_talk_AssertionFailure_t *)CrankAllocateValuesObjectWithClassName(primContext->context, AssertionFailure);
        assertionFailure->super.super.messageText = crank_talk_String_createWithCString(primContext->context, "Assertion failed");
        crank_talk_Exception_signal(primContext->context, &assertionFailure->super.super);
        abort();
    }

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_Object_assertWithDescription_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t assertValue = primContext->arguments[1];
    crank_talk_oop_t description = primContext->arguments[2];
    if(assertValue != primContext->context->roots.trueValue)
    {        
        crank_talk_AssertionFailure_t *assertionFailure = (crank_talk_AssertionFailure_t *)CrankAllocateValuesObjectWithClassName(primContext->context, AssertionFailure);
        assertionFailure->super.super.messageText = description;
        crank_talk_Exception_signal(primContext->context, &assertionFailure->super.super);
        abort();
    }

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_Object_error_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    //crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t errorDescription = primContext->arguments[1];

    crank_talk_Error_t *error = (crank_talk_Error_t *)CrankAllocateValuesObjectWithClassName(primContext->context, Error);
    error->super.messageText = errorDescription;
    crank_talk_Exception_signal(primContext->context, &error->super);
    abort();
}

static void
crank_talk_Object_subclassResponsibility_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    //crank_talk_oop_t self = primContext->arguments[0];

    crank_talk_SubclassResponsibility_t *error = (crank_talk_SubclassResponsibility_t *)CrankAllocateValuesObjectWithClassName(primContext->context, SubclassResponsibility);
    error->super.super.messageText = crank_talk_String_createWithCString(primContext->context, "SubclassResponsibility");
    error->selector = crank_talk_Symbol_internWithCString(primContext->context, "subclassResponsibility");
    crank_talk_Exception_signal(primContext->context, &error->super.super);
    abort();
}


static void
crank_talk_BlockClosure_onDo_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t exceptionClass = primContext->arguments[1];
    crank_talk_oop_t exceptionAction = primContext->arguments[2];

    if(!crank_talk_oop_isKindOf(primContext->context, exceptionClass, primContext->context->roots.BehaviorClass))
        crank_talk_error(primContext->context, "Expected a behavior for on:do:");

    if(!crank_talk_oop_isKindOf(primContext->context, exceptionAction, primContext->context->roots.BlockClosureClass))
        crank_talk_error(primContext->context, "Expected a block closure for on:do:");

    crank_talk_stackFrameOnDoRecord_t onDoRecord = {
        .super.type = CrankTalkStackFrameRecordOnDoRecord,
        .exceptionClass = exceptionClass,
        .exceptionAction = exceptionAction,
        .result = crank_talk_oop_nil()
    };

    crank_talk_stackFrame_pushRecord(&onDoRecord.super);

    if(!_setjmp(onDoRecord.jmpbuffer))
        onDoRecord.result = crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, self);

    crank_talk_stackFrame_popRecord(&onDoRecord.super);
    return crank_talk_Primitive_return(primContext, onDoRecord.result);
}

static void
crank_talk_BlockClosure_ensure_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t ensureBlock = primContext->arguments[1];

    crank_talk_stackFrameEnsureRecord_t ensureRecord = {
        .super.type = CrankTalkStackFrameRecordEnsureRecord,
        .ensureBlock = ensureBlock,
    };

    crank_talk_stackFrame_pushRecord(&ensureRecord.super);

    crank_talk_oop_t result = crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, self);

    crank_talk_stackFrame_popRecord(&ensureRecord.super);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_Exception_signal_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_Exception_t *exception = (crank_talk_Exception_t *)primContext->arguments[0];
    crank_talk_Exception_signal(primContext->context, exception);
    return crank_talk_Primitive_return(primContext, exception->resumeValue);
}

static void
crank_talk_Exception_signalWith_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_Exception_t *exception = (crank_talk_Exception_t *)primContext->arguments[0];
    exception->messageText = primContext->arguments[1];

    crank_talk_Exception_signal(primContext->context, exception);
    return crank_talk_Primitive_return(primContext, exception->resumeValue);
}

void
crank_talk_registerErrorPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "assert:", 2, "Object::assert:", crank_talk_Object_assert_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "assert:description:", 3, "Object::assert:description:", crank_talk_Object_assertWithDescription_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "error:", 2, "Object::error:", crank_talk_Object_error_primitive);
    
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ObjectClass, "subclassResponsibility", 1, "Object::subclassResponsibility", crank_talk_Object_subclassResponsibility_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "on:do:", 3, "BlockClosure::on:do:", crank_talk_BlockClosure_onDo_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "ensure:", 2, "BlockClosure::ensure:", crank_talk_BlockClosure_ensure_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ExceptionClass, "signal", 1, "Exception::signal", crank_talk_Exception_signal_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ExceptionClass, "signal:", 2, "Exception::signal:", crank_talk_Exception_signalWith_primitive);
}