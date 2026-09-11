#include "crank-talk/function.h"
#include "crank-talk/analysis-build.h"
#include "crank-talk/array.h"
#include "crank-talk/bytecode.h"
#include "crank-talk/context.h"
#include "crank-talk/environment.h"
#include "crank-talk/errors.h"
#include "crank-talk/hir.h"
#include "crank-talk/parse-tree.h"
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

static void
crank_talk_CompiledFunction_ensureHIRAnalysis(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction)
{
    if(!crank_talk_oop_isNil(compiledFunction->hirFunction))
        return;

    if(!compiledFunction->definitionBody)
        return;

    compiledFunction->hirFunction = CrankAllocateValuesObjectWithClassName(context, HIRFunction);
    crank_talk_HIRFunction_t *hirFunction = (crank_talk_HIRFunction_t*)compiledFunction->hirFunction;

    crank_talk_oop_t sourcePosition = (crank_talk_oop_t)((crank_talk_ParseTreeNode_t*)compiledFunction->definitionBody)->sourcePosition;

    struct {
        crank_talk_HIRBasicBlock_t *allocaBlock;
        crank_talk_HIRBuilder_t *allocaBuilder;
        crank_talk_HIRBasicBlock_t *entryBlock;
        crank_talk_HIRBuilder_t *builder;
        crank_talk_oop_t lexicalEnvironment;
        crank_talk_oop_t analysisAndBuildContext;
        crank_talk_HIRValue_t * result;
    } gcFrame = {0};

    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Create the function analysis environment.
    gcFrame.lexicalEnvironment = crank_talk_FunctionAnalysisEnvironment_create(context, compiledFunction->definitionEnvironment);

    // Arguments
    {
        size_t argumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)compiledFunction->definitionArguments);
        crank_talk_oop_t *definitionArguments = compiledFunction->definitionArguments->elements;

        hirFunction->arguments = (crank_talk_Array_t*)crank_talk_Array_create(context, argumentCount);
        crank_talk_oop_t *hirArguments = hirFunction->arguments->elements;
        for(size_t i = 0; i < argumentCount; ++i)
        {
            crank_talk_ParseTreeArgumentDefinitionNode_t *argumentDefinition = (crank_talk_ParseTreeArgumentDefinitionNode_t*)definitionArguments[i];
            crank_talk_oop_t hirArgumentValue = CrankAllocateValuesObjectWithClassName(context, HIRArgument);
            hirArguments[i] = hirArgumentValue;

            crank_talk_HIRArgument_t *hirArgument = (crank_talk_HIRArgument_t*)hirArgumentValue;
            hirArgument->super.super.sourcePosition = (crank_talk_oop_t)argumentDefinition->super.sourcePosition;
            hirArgument->super.name = argumentDefinition->name;
            if(argumentDefinition->name)
                crank_talk_LexicalEnvironment_setNewHIRSymbolBinding(context, gcFrame.lexicalEnvironment, (crank_talk_oop_t)argumentDefinition->name, hirArgumentValue, (crank_talk_oop_t)argumentDefinition->super.sourcePosition);
        }

    }

    // Alloca builder
    gcFrame.allocaBlock = crank_talk_HIRBasicBlock_createNamed(context, "alloca");
    gcFrame.allocaBuilder = crank_talk_HIRBuilder_create(context, hirFunction, gcFrame.allocaBlock, NULL, gcFrame.lexicalEnvironment);
    crank_talk_HIRFunction_addBasicBlock(hirFunction, gcFrame.allocaBlock);

    // Entry block and builder
    gcFrame.entryBlock = crank_talk_HIRBasicBlock_createNamed(context, "entry");
    gcFrame.builder = crank_talk_HIRBuilder_create(context, hirFunction, gcFrame.entryBlock, gcFrame.allocaBuilder, gcFrame.lexicalEnvironment);
    crank_talk_HIRFunction_addBasicBlock(hirFunction, gcFrame.entryBlock);

    // Analyze and build
    gcFrame.analysisAndBuildContext = crank_talk_createAnalysisAndBuildContext(context, gcFrame.builder);
    gcFrame.result = crank_talk_ParseTreeAnalyzeAndBuild(context, compiledFunction->definitionBody, gcFrame.analysisAndBuildContext);

    if(!crank_talk_HIRBuilder_isLastTerminator(context, gcFrame.builder))
    {
        if(crank_talk_oop_asBoolean(context, compiledFunction->isBlock))
        {
            // Return the result.
            crank_talk_HIRBuilder_return(context, gcFrame.builder, gcFrame.result, sourcePosition);
        }
        else
        {
            // TODO: return self.
            printf("TODO: return self.\n");
            crank_talk_HIRBuilder_unreachable(context, gcFrame.builder, sourcePosition);
        }
    }

    // Jump from the alloca block into the entry block.
    crank_talk_HIRBuilder_branch(context, gcFrame.allocaBuilder, gcFrame.entryBlock, sourcePosition);

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    
}

static void
crank_talk_CompiledFunction_ensureBytecodeGenerationAnalysis(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction)
{
    assert(compiledFunction->hirFunction);
    if(compiledFunction->bytecode)
        return;

    struct {
        crank_talk_BytecodeAssembler_t *assembler;
    } gcFrame = {0};
    
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.assembler = crank_talk_BytecodeAssembler_create(context);

    crank_talk_HIRFunction_t *hirFunction = (crank_talk_HIRFunction_t *)compiledFunction->hirFunction;

    // Arguments
    {
        size_t argumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)hirFunction->arguments);
        crank_talk_oop_t *arguments = hirFunction->arguments->elements;
        for(size_t i = 0; i < argumentCount; ++i)
        {
            crank_talk_HIRArgument_t *hirArgument = (crank_talk_HIRArgument_t*)arguments[i];
            hirArgument->super.bytecodeVectorIndex = crank_talk_oop_encodeSmallInteger(crank_talk_BytecodeAssembler_getArgumentOperand(i));
        }
    }

    // Captures
    {
        size_t captureCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)hirFunction->captures);
        crank_talk_oop_t *captures = hirFunction->captures->elements;
        for(size_t i = 0; i < captureCount; ++i)
        {
            crank_talk_HIRCapture_t *hirArgument = (crank_talk_HIRCapture_t*)captures[i];
            hirArgument->super.bytecodeVectorIndex = crank_talk_oop_encodeSmallInteger(crank_talk_BytecodeAssembler_getCaptureOperand(i));
        }
    }

    // Basic blocks labels and temporaries.
    {
        crank_talk_HIRBasicBlock_t *basicBlock = hirFunction->firstBasicBlock;
        while(basicBlock)
        {
            basicBlock->bytecodeLabel = (crank_talk_oop_t)crank_talk_BytecodeAssembler_newLabel(context);

            crank_talk_HIRInstruction_t *instruction = basicBlock->firstInstruction;
            while(instruction)
            {
                if(crank_talk_HIRInstruction_hasResultValue(context, instruction))
                    instruction->super.bytecodeVectorIndex = crank_talk_oop_encodeSmallInteger(crank_talk_BytecodeAssembler_newTemporary(context, gcFrame.assembler));
                else
                    instruction->super.bytecodeVectorIndex = crank_talk_oop_encodeSmallInteger(0);
                instruction = instruction->nextInstruction;
            }

            basicBlock = basicBlock->nextBlock;
        }
    }

    // Basic block construction
    {
        crank_talk_HIRBasicBlock_t *basicBlock = hirFunction->firstBasicBlock;
        while(basicBlock)
        {
            crank_talk_BytecodeAssembler_putLabel(context, gcFrame.assembler, (crank_talk_BytecodeLabel_t*)basicBlock->bytecodeLabel);
            crank_talk_HIRInstruction_t *instruction = basicBlock->firstInstruction;
            while(instruction)
            {
                crank_talk_HIRInstruction_emitBytecodeAssemblyWith(context, instruction, (crank_talk_oop_t)gcFrame.assembler);

                instruction = instruction->nextInstruction;
            }

            basicBlock = basicBlock->nextBlock;
        }
    }

    crank_talk_BytecodeAssembler_finishBuildingInFunction(context, gcFrame.assembler, compiledFunction);
    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
}

CRANK_TALK_API void
crank_talk_CompiledFunction_ensureAnalysis(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction)
{
    if(compiledFunction->hirFunction || compiledFunction->bytecode)
        return;

    crank_talk_CompiledFunction_ensureHIRAnalysis(context, compiledFunction);
    crank_talk_CompiledFunction_ensureBytecodeGenerationAnalysis(context, compiledFunction);
}

static void
crank_talk_CompiledFunction_dumpToConsole(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction)
{
    crank_talk_CompiledFunction_ensureAnalysis(context, compiledFunction);
    if(compiledFunction->hirFunction)
        crank_talk_HIRFunction_dumpToConsole(context, (crank_talk_HIRFunction_t*)compiledFunction->hirFunction);
    if(compiledFunction->bytecode)
        crank_talk_BytecodeFunction_dumpToConsole(context, compiledFunction);
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
