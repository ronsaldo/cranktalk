#include "crank-talk/function.h"
#include "crank-talk/analysis-build.h"
#include "crank-talk/analysis-evaluation.h"
#include "crank-talk/array.h"
#include "crank-talk/bytecode.h"
#include "crank-talk/context.h"
#include "crank-talk/environment.h"
#include "crank-talk/errors.h"
#include "crank-talk/hir.h"
#include "crank-talk/message.h"
#include "crank-talk/ordered-collection.h"
#include "crank-talk/parse-tree.h"
#include "crank-talk/stack-frame.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

CRANK_TALK_API crank_talk_CompiledFunction_t*
crank_talk_CompiledFunction_createForNativePrimitive(crank_talk_VMContext_t *context, crank_talk_oop_t name, size_t argumentCount, crank_talk_CompiledFunctionNativeEntryPoint_t entryPoint)
{
    crank_talk_ExternalAddress_t *entryPointAddress = (crank_talk_ExternalAddress_t*)crank_talk_ExternalAddress_make(context, entryPoint);

    crank_talk_CompiledFunction_t *primitiveFunction = (crank_talk_CompiledFunction_t*)CrankAllocateValuesObjectWithClassName(context, CompiledFunction);
    primitiveFunction->name = name;
    primitiveFunction->argumentCount = crank_talk_oop_encodeSmallInteger(argumentCount);
    primitiveFunction->nativeEntryPoint = entryPointAddress;
    return primitiveFunction; 
}

CRANK_TALK_API crank_talk_CompiledFunction_t*
crank_talk_CompiledFunction_createForPrimitive(crank_talk_VMContext_t *context, crank_talk_oop_t name, size_t argumentCount, const char *primitiveName, crank_talk_PrimitiveFunction_t primitiveEntryPoint)
{

    crank_talk_CompiledFunction_t *primitiveFunction = (crank_talk_CompiledFunction_t*)CrankAllocateValuesObjectWithClassName(context, CompiledFunction);
    primitiveFunction->name = name;
    primitiveFunction->argumentCount = crank_talk_oop_encodeSmallInteger(argumentCount);
    primitiveFunction->primitiveName = (crank_talk_Symbol_t*)crank_talk_Symbol_internWithCString(context, primitiveName);

    if(primitiveEntryPoint)
    {
        crank_talk_ExternalAddress_t *entryPointAddress = (crank_talk_ExternalAddress_t*)crank_talk_ExternalAddress_make(context, primitiveEntryPoint);
        primitiveFunction->primitiveEntryPoint = entryPointAddress;
    }

    return primitiveFunction; 
}

crank_talk_oop_t
crank_talk_CompiledFunction_apply(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *function, crank_talk_BlockClosure_t *closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    if(!crank_talk_oop_isNonNullPointer((crank_talk_oop_t)function))
        crank_talk_error(context, "Expected a valid function.");

    size_t expectedArgumentCount = (size_t)crank_talk_oop_decodeSmallInteger(function->argumentCount);
    if(argumentCount != expectedArgumentCount)
    {
        crank_talk_errorPrintf(context, "Function "CRANK_TALK_STRING_FORMAT" expects %d arguments instead of %d.",
            CRANK_TALK_STRING_FORMAT_ARG(function->name),
            (int)expectedArgumentCount, (int)argumentCount);
    }

    crank_talk_stackFrameApplicationArgumentsRecord_t rootsRecord = {
        .super.type = CrankTalkStackFrameRecordApplicationArguments,
        .functional = (crank_talk_oop_t)function,
        .blockClosure = (crank_talk_oop_t)closure,
        .argumentCount = argumentCount,
        .arguments = arguments
    };

    crank_talk_stackFrame_pushRecord(&rootsRecord.super);

    crank_talk_CompiledFunction_ensureAnalysis(context, function);

    if(function->primitiveName)
    {
        // Find the primitive entry point
        if(!function->primitiveEntryPoint)
        {
            crank_talk_PrimitiveFunction_t entryPoint = crank_talk_PrimitiveTables_lookupWithString(&context->primitiveTable, crank_talk_object_getSizeInBytes((crank_talk_oop_t)function->primitiveName), function->primitiveName->elements);
            if(entryPoint)
            {
                function->primitiveEntryPoint = (crank_talk_ExternalAddress_t*)crank_talk_ExternalAddress_make(context, entryPoint);
            }
        }

        // Evaluate the primitive.
        if(function->primitiveEntryPoint)
        {
            crank_talk_PrimitiveContext_t primitiveContext = {
                .context = context,
                .closure = (crank_talk_oop_t)closure,
                .argumentCount = argumentCount,
                .arguments = arguments,
                .resultValue = crank_talk_oop_nil(),
                .failed = false,
            };
        
            crank_talk_PrimitiveFunction_t entryPoint = (crank_talk_PrimitiveFunction_t)function->primitiveEntryPoint->address;
            entryPoint(&primitiveContext);

            if(!primitiveContext.failed)
            {
                crank_talk_stackFrame_popRecord(&rootsRecord.super);
                return primitiveContext.resultValue;
            }
        }
    }


    if(function->nativeEntryPoint)
    {
        crank_talk_CompiledFunctionNativeEntryPoint_t entryPoint = (crank_talk_CompiledFunctionNativeEntryPoint_t)function->nativeEntryPoint->address;
        crank_talk_oop_t result = entryPoint(context, (crank_talk_oop_t)closure, argumentCount, arguments);
        crank_talk_stackFrame_popRecord(&rootsRecord.super);
        return result;
    }

    crank_talk_stackFrame_popRecord(&rootsRecord.super);

    return crank_talk_vm_BytecodeFunction_apply(context, function, closure, argumentCount, arguments);
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

    crank_talk_error(context, "TODO: non functional application.");
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
        crank_talk_Array_t *pragmas;
        crank_talk_oop_t pragmaEvaluationContext;
        crank_talk_HIRBasicBlock_t *allocaBlock;
        crank_talk_HIRBuilder_t *allocaBuilder;
        crank_talk_HIRBasicBlock_t *entryBlock;
        crank_talk_HIRBuilder_t *builder;
        crank_talk_oop_t lexicalEnvironment;
        crank_talk_oop_t analysisAndBuildContext;
        crank_talk_HIRValue_t *result;
        crank_talk_HIRValue_t *receiverValue;
    } gcFrame = {0};

    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Evaluate the pragmas
    if(crank_talk_isParseTreeSequenceNode(context, compiledFunction->definitionBody))
    {
        crank_talk_ParseTreeSequenceNode_t *sequenceNode = (crank_talk_ParseTreeSequenceNode_t *)compiledFunction->definitionBody;
        size_t pragmaCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->pragmas);
        gcFrame.pragmas = (crank_talk_Array_t*)crank_talk_Array_create(context, pragmaCount);
        if(pragmaCount > 0)
        {
            gcFrame.pragmaEvaluationContext = crank_talk_createTopLevelAnalysisAndEvaluationContext(context, crank_talk_oop_nil());
            for(size_t i = 0; i < pragmaCount; ++i)
            {
                crank_talk_oop_t pragmaNode = sequenceNode->pragmas->elements[i];
                crank_talk_Pragma_t *pragma = (crank_talk_Pragma_t *)crank_talk_ParseTreeAnalyzeAndEvaluate(context, pragmaNode, gcFrame.pragmaEvaluationContext);
                gcFrame.pragmas->elements[i] = (crank_talk_oop_t)pragma;

                if((crank_talk_oop_t)pragma->selector == context->roots.primitiveSymbol
                && crank_talk_object_getSizeInSlots((crank_talk_oop_t)pragma->arguments) == 1)
                {
                    crank_talk_oop_t primitiveName = pragma->arguments->elements[0];
                    if(crank_talk_oop_isKindOf(context, primitiveName, context->roots.SymbolClass))
                        compiledFunction->primitiveName = (crank_talk_Symbol_t*)primitiveName;
                }
                
            }
            
        }
    }
    else
    {
        gcFrame.pragmas = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    }

    compiledFunction->pragmas = gcFrame.pragmas;

    // Create the function analysis environment.
    gcFrame.lexicalEnvironment = crank_talk_FunctionAnalysisEnvironment_create(context, compiledFunction->definitionEnvironment);
    crank_talk_FunctionAnalysisEnvironment_t *functionAnalysisEnvironment = (crank_talk_FunctionAnalysisEnvironment_t *)gcFrame.lexicalEnvironment;
    functionAnalysisEnvironment->receiverClass = (crank_talk_Behavior_t*)compiledFunction->owner;
    functionAnalysisEnvironment->isInnerBlock = compiledFunction->isInnerBlock;

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

            if(crank_talk_oop_asBoolean(context, argumentDefinition->isSelf))
            {
                gcFrame.receiverValue = &hirArgument->super.super;
            }
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
            // Return self.
            if(!gcFrame.receiverValue)
                crank_talk_errorAtSourcePosition(context, sourcePosition, "Method is missing a self argument.");
            crank_talk_HIRBuilder_return(context, gcFrame.builder, gcFrame.receiverValue, sourcePosition);
        }
    }

    // Jump from the alloca block into the entry block.
    crank_talk_HIRBuilder_branch(context, gcFrame.allocaBuilder, gcFrame.entryBlock, sourcePosition);

    // Captures
    hirFunction->captures = (crank_talk_Array_t*)crank_talk_OrderedCollection_asArray(context, functionAnalysisEnvironment->captureList);

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
    if(compiledFunction->hirFunction || compiledFunction->bytecode || compiledFunction->nativeEntryPoint || compiledFunction->primitiveEntryPoint)
        return;

    crank_talk_CompiledFunction_ensureHIRAnalysis(context, compiledFunction);
    crank_talk_CompiledFunction_ensureBytecodeGenerationAnalysis(context, compiledFunction);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_CompiledFunction_getSourcePositionAtPC(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction, size_t pc)
{
    (void)context;
    if(!compiledFunction->sourcePositions)
        return crank_talk_oop_nil();

    // TODO: Perform a lower bound binary search.
    size_t sourcePositionCounts = crank_talk_object_getSizeInSlots((crank_talk_oop_t)compiledFunction->sourcePositions) / 2;
    crank_talk_oop_t bestFound = crank_talk_oop_nil();
    for(size_t i = 0; i < sourcePositionCounts; ++i)
    {
        size_t positionPC = crank_talk_oop_decodeSmallInteger(compiledFunction->sourcePositions->elements[i*2]);
        if(positionPC <= pc)
            bestFound = compiledFunction->sourcePositions->elements[i*2 + 1];
        else
            break;
    }
    
    return bestFound;
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
        crank_talk_error(context, "Expected a functional for dumping.");
}

static void
crank_talk_BlockClosure_value_primitives(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t result = crank_talk_apply(primContext->context, self, primContext->argumentCount - 1, primContext->arguments + 1);
    return crank_talk_Primitive_return(primContext, result);
}

void
crank_talk_registerFunctionalPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "value",                    1, "BlockClosure::value",                    crank_talk_BlockClosure_value_primitives);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "value:",                   2, "BlockClosure::value:",                   crank_talk_BlockClosure_value_primitives);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "value:value:",             3, "BlockClosure::value:value:",             crank_talk_BlockClosure_value_primitives);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "value:value:value:",       4, "BlockClosure::value:value:value:",       crank_talk_BlockClosure_value_primitives);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "value:value:value:value:", 5, "BlockClosure::value:value:value:value:", crank_talk_BlockClosure_value_primitives);
}
