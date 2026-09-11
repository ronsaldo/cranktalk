#include "crank-talk/function.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/hir.h"
#include "crank-talk/stack-frame.h"
#include <stdlib.h>
#include <stdio.h>

CRANK_TALK_API crank_talk_CompiledFunction_t*
crank_talk_CompiledFunction_createForPrimitive(crank_talk_VMContext_t *context, crank_talk_oop_t name, size_t argumentCount, crank_talk_CompiledFunctionNativeEntryPoint_t entryPoint)
{
    crank_talk_ExternalAddress_t *entryPointAddress = (crank_talk_ExternalAddress_t*)CrankAllocateBytesObjectWithClassName(context, ExternalAddress);
    entryPointAddress->address = entryPoint;

    crank_talk_CompiledFunction_t *primitiveFunction = (crank_talk_CompiledFunction_t*)CrankAllocateValuesObjectWithClassName(context, CompiledFunction);
    primitiveFunction->name = name;
    primitiveFunction->argumentCount = crank_talk_oop_encodeSmallInteger(argumentCount);
    primitiveFunction->nativeEntryPoint = entryPointAddress;
    return primitiveFunction; 

}

crank_talk_oop_t
crank_talk_CompiledFunction_apply(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *function, crank_talk_BlockClosure_t *closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    if(!crank_talk_oop_isNonNullPointer((crank_talk_oop_t)function))
        crank_talk_error("Expected a valid function.");

    crank_talk_stackFrameApplicationArgumentsRecord_t rootsRecord = {
        .super.type = CrankTalkStackFrameRecordApplicationArguments,
        .functional = (crank_talk_oop_t)function,
        .blockClosure = (crank_talk_oop_t)closure,
        .argumentCount = argumentCount,
        .arguments = arguments
    };

    crank_talk_stackFrame_pushRecord(&rootsRecord.super);

    if(function->nativeEntryPoint)
    {
        crank_talk_CompiledFunctionNativeEntryPoint_t entryPoint = (crank_talk_CompiledFunctionNativeEntryPoint_t)function->nativeEntryPoint->address;
        crank_talk_oop_t result = entryPoint(context, (crank_talk_oop_t)closure, argumentCount, arguments);
        crank_talk_stackFrame_popRecord(&rootsRecord.super);
        return result;
    }

    crank_talk_error("TODO: interpreted or lazy definition function.");
    abort();
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_apply(crank_talk_VMContext_t *context, crank_talk_oop_t function, size_t argumentCount, crank_talk_oop_t *arguments)
{
    crank_talk_oop_t functionClass = (crank_talk_oop_t)crank_talk_object_getBehavior(function);
    if(functionClass == context->roots.CompiledFunctionClass)
    {
        return crank_talk_CompiledFunction_apply(context, (crank_talk_CompiledFunction_t*)function, NULL, argumentCount, arguments);
    }
    else if(functionClass == context->roots.BlockClosureClass)
    {
        crank_talk_BlockClosure_t *blockClosure = (crank_talk_BlockClosure_t*)function;
        return crank_talk_CompiledFunction_apply(context, blockClosure->function, blockClosure, argumentCount, arguments);
    }

    crank_talk_error("TODO: non functional application.");
    abort();
}

CRANK_TALK_API void
sysmel_vm_CompiledFunction_ensureHIRAnalysis(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction)
{
    if(!crank_talk_oop_isNil(compiledFunction->hirFunction))
        return;

    compiledFunction->hirFunction = CrankAllocateValuesObjectWithClassName(context, HIRFunction);
    crank_talk_HIRFunction_t *hirFunction = (crank_talk_HIRFunction_t*)compiledFunction->hirFunction;

    struct {
        crank_talk_HIRBasicBlock_t *allocaBlock;
        crank_talk_HIRBuilder_t *allocaBuilder;
        crank_talk_HIRBasicBlock_t *entryBlock;
        crank_talk_HIRBuilder_t *builder;
    } gcFrame = {0};

    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Alloca builder
    gcFrame.allocaBlock = crank_talk_HIRBasicBlock_createNamed(context, "alloca");
    crank_talk_HIRFunction_addBasicBlock(hirFunction, gcFrame.allocaBlock);

    // Entry block and builder
    gcFrame.entryBlock = crank_talk_HIRBasicBlock_createNamed(context, "entry");
    crank_talk_HIRFunction_addBasicBlock(hirFunction, gcFrame.entryBlock);

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    
}

CRANK_TALK_API void
crank_talk_CompiledFunction_ensureAnalysis(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction)
{
    if(!crank_talk_oop_isNil(compiledFunction->hirFunction))
        return;

    sysmel_vm_CompiledFunction_ensureHIRAnalysis(context, compiledFunction);
}

static void
crank_talk_CompiledFunction_dumpToConsole(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction)
{
    crank_talk_CompiledFunction_ensureAnalysis(context, compiledFunction);
    if(compiledFunction->hirFunction)
        crank_talk_HIRFunction_dumpToConsole(context, (crank_talk_HIRFunction_t*)compiledFunction->hirFunction);
}

static void
crank_talk_BlockClosure_dumpToConsole(crank_talk_VMContext_t *context, crank_talk_BlockClosure_t *blockClosure)
{
    crank_talk_CompiledFunction_dumpToConsole(context, blockClosure->function);
}

CRANK_TALK_API void
crank_talk_Functional_dumpToConsole(crank_talk_VMContext_t *context, crank_talk_oop_t functional)
{
    if(crank_talk_oop_isKindOf(context, functional, context->roots.CompiledFunctionClass))
        crank_talk_CompiledFunction_dumpToConsole(context, (crank_talk_CompiledFunction_t*)functional);
    else if(crank_talk_oop_isKindOf(context, functional, context->roots.BlockClosureClass))
        crank_talk_BlockClosure_dumpToConsole(context, (crank_talk_BlockClosure_t*)functional);
    else
        crank_talk_error("Expected a functinal for dumping.");
}
