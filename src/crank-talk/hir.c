#include "crank-talk/hir.h"
#include "crank-talk/bytecode.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include "crank-talk/ordered-collection.h"
#include "crank-talk/stack-frame.h"
#include "crank-talk/string.h"
#include <stdio.h>
#include <stdlib.h>

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRConstantLiteralValue_create(crank_talk_VMContext_t *context, crank_talk_oop_t value, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t constantLiteralValue = CrankAllocateValuesObjectWithClassName(context, HIRConstantLiteralValue);
    crank_talk_HIRConstantLiteralValue_t *literalValue = (crank_talk_HIRConstantLiteralValue_t*)constantLiteralValue;
    literalValue->super.super.sourcePosition = sourcePosition;
    literalValue->value = value;
    return constantLiteralValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRConstantControlFlowEscapeValue_create(crank_talk_VMContext_t *context, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t controlFlowEscape = CrankAllocateValuesObjectWithClassName(context, HIRConstantControlFlowEscapeValue);
    crank_talk_HIRConstantControlFlowEscapeValue_t *literalValue = (crank_talk_HIRConstantControlFlowEscapeValue_t*)controlFlowEscape;
    literalValue->super.super.sourcePosition = sourcePosition;
    return controlFlowEscape;
}

CRANK_TALK_API void
crank_talk_HIRFunction_ensureEnumeratedLocalValues(crank_talk_VMContext_t *context, crank_talk_HIRFunction_t *function)
{
    if(function->enumeratedLocalValues)
        return;

    crank_talk_oop_t localValues = crank_talk_OrderedCollection_create(context);

    // Arguments
    {
        size_t argumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)function->arguments);
        crank_talk_oop_t *arguments = function->arguments->elements;
        for(size_t i = 0; i < argumentCount; ++i)
        {
            crank_talk_HIRArgument_t *argument = (crank_talk_HIRArgument_t *)arguments[i];
            crank_talk_OrderedCollection_add(context, localValues, arguments[i]);
            argument->super.index = crank_talk_oop_encodeSmallInteger(crank_talk_OrderedCollection_getSize(localValues));

        }
    }

    // Captures
    {
        size_t captureCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)function->captures);
        crank_talk_oop_t *captures = function->captures->elements;
        for(size_t i = 0; i < captureCount; ++i)
        {
            crank_talk_HIRCapture_t *capture = (crank_talk_HIRCapture_t *)captures[i];
            crank_talk_OrderedCollection_add(context, localValues, captures[i]);
            capture->super.index = crank_talk_oop_encodeSmallInteger(crank_talk_OrderedCollection_getSize(localValues));
        }
    }

    // Basic blocks
    crank_talk_HIRBasicBlock_t *basicBlock = function->firstBasicBlock;
    while(basicBlock)
    {
        crank_talk_OrderedCollection_add(context, localValues, (crank_talk_oop_t)basicBlock);
        basicBlock->super.index = crank_talk_oop_encodeSmallInteger(crank_talk_OrderedCollection_getSize(localValues));

        // Instructions
        crank_talk_HIRInstruction_t *instruction = basicBlock->firstInstruction;
        while(instruction)
        {
            crank_talk_OrderedCollection_add(context, localValues, (crank_talk_oop_t)instruction);
            instruction->super.index = crank_talk_oop_encodeSmallInteger(crank_talk_OrderedCollection_getSize(localValues));

            instruction = instruction->nextInstruction;
        }


        // Next block
        basicBlock = basicBlock->nextBlock;
    }

    function->enumeratedLocalValues = (crank_talk_Array_t*)crank_talk_OrderedCollection_asArray(context, localValues);
}

CRANK_TALK_API void
crank_talk_HIRFunction_dumpToStream(crank_talk_VMContext_t *context, crank_talk_HIRFunction_t *function, crank_talk_oop_t stringStream)
{
    crank_talk_HIRFunction_ensureEnumeratedLocalValues(context, function);

    // Header
    crank_talk_StringStream_insertCString(context, stringStream, "function");
    if(function->name)
    {
        crank_talk_StringStream_insertCString(context, stringStream, " ");
        crank_talk_StringStream_insertBytesObject(context, stringStream, function->name);
    }
    
    crank_talk_StringStream_insertCString(context, stringStream, " {\n");

    // Arguments
    {
        size_t argumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)function->arguments);
        crank_talk_oop_t *arguments = function->arguments->elements;
        for(size_t i = 0; i < argumentCount; ++i)
        {
            crank_talk_StringStream_insertCString(context, stringStream, "  argument ");
            crank_talk_StringStream_print(context, stringStream, arguments[i]);
            crank_talk_StringStream_insertCString(context, stringStream, "\n");
        }
    }

    // Captures
    {
        size_t captureCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)function->captures);
        crank_talk_oop_t *captures = function->captures->elements;
        for(size_t i = 0; i < captureCount; ++i)
        {
            crank_talk_StringStream_insertCString(context, stringStream, "  capture ");
            crank_talk_StringStream_print(context, stringStream, captures[i]);
            crank_talk_StringStream_insertCString(context, stringStream, "\n");
        }
    }

    // Basic blocks
    crank_talk_HIRBasicBlock_t *basicBlock = function->firstBasicBlock;
    while(basicBlock)
    {
        crank_talk_StringStream_insertCString(context, stringStream, "basicBlock ");
        crank_talk_StringStream_print(context, stringStream, (crank_talk_oop_t)basicBlock);
        crank_talk_StringStream_insertCString(context, stringStream, ":\n");

        // Instructions
        crank_talk_HIRInstruction_t *instruction = basicBlock->firstInstruction;
        while(instruction)
        {
            crank_talk_StringStream_insertCString(context, stringStream, "    ");
            crank_talk_StringStream_fullPrint(context, stringStream, (crank_talk_oop_t)instruction);
            crank_talk_StringStream_insertCString(context, stringStream, "\n");

            instruction = instruction->nextInstruction;
        }

        basicBlock = basicBlock->nextBlock;
    }

    // Footer
    crank_talk_StringStream_insertCString(context, stringStream, "}\n");
}

CRANK_TALK_API void
crank_talk_HIRFunction_dumpToConsole(crank_talk_VMContext_t *context, crank_talk_HIRFunction_t *functionValue)
{
    struct {
        crank_talk_HIRFunction_t *functionValue;
        crank_talk_oop_t stringStream;
    } gcFrame = {0};

    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);
    gcFrame.functionValue = functionValue;
    gcFrame.stringStream = crank_talk_StringStream_create(context);
    crank_talk_HIRFunction_dumpToStream(context, functionValue, gcFrame.stringStream);

    crank_talk_oop_t string = crank_talk_StringStream_asString(context, gcFrame.stringStream);
    printf(CRANK_TALK_STRING_FORMAT, CRANK_TALK_STRING_FORMAT_ARG(string));

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);

}

CRANK_TALK_API void
crank_talk_HIRFunction_addBasicBlock(crank_talk_HIRFunction_t *function, crank_talk_HIRBasicBlock_t *basicBlock)
{
    if(function->firstBasicBlock)
    {
        function->lastBasicBlock->nextBlock = basicBlock;
        basicBlock->previousBlock = function->lastBasicBlock;
        function->lastBasicBlock = basicBlock;

    }
    else
    {
        function->firstBasicBlock = function->lastBasicBlock = basicBlock;
    }
}

CRANK_TALK_API void
crank_talk_HIRBasicBlock_addInstruction(crank_talk_HIRBasicBlock_t *basicBlock, crank_talk_HIRInstruction_t *instruction)
{
    if(basicBlock->firstInstruction)
    {
        basicBlock->lastInstruction->nextInstruction = instruction;
        instruction->previousInstruction = basicBlock->lastInstruction;
        basicBlock->lastInstruction = instruction;
    }
    else
    {
        basicBlock->firstInstruction = basicBlock->lastInstruction = instruction;
    }
}

CRANK_TALK_API crank_talk_HIRBasicBlock_t*
crank_talk_HIRBasicBlock_create(crank_talk_VMContext_t *context)
{
    return (crank_talk_HIRBasicBlock_t*)CrankAllocateValuesObjectWithClassName(context, HIRBasicBlock);
}

CRANK_TALK_API crank_talk_HIRBasicBlock_t*
crank_talk_HIRBasicBlock_createNamed(crank_talk_VMContext_t *context, const char *name)
{
    crank_talk_HIRBasicBlock_t *basicBlock = crank_talk_HIRBasicBlock_create(context);
    basicBlock->super.name = (crank_talk_Symbol_t*)crank_talk_Symbol_internWithCString(context, name);
    return basicBlock;
}

CRANK_TALK_API bool
crank_talk_HIRInstruction_hasResultValue(crank_talk_VMContext_t *context, crank_talk_HIRInstruction_t *instruction)
{
    return crank_talk_oop_asBoolean(context, crank_talk_send0(context, context->roots.hasResultValueSymbol, (crank_talk_oop_t)instruction));
}

CRANK_TALK_API crank_talk_HIRBuilder_t *
crank_talk_HIRBuilder_create(crank_talk_VMContext_t *context, crank_talk_HIRFunction_t *function, crank_talk_HIRBasicBlock_t *basicBlock, crank_talk_HIRBuilder_t *allocaBuilder, crank_talk_oop_t lexicalEnvironment)
{
    crank_talk_HIRBuilder_t *builder = (crank_talk_HIRBuilder_t *)CrankAllocateValuesObjectWithClassName(context, HIRBuilder);
    builder->hirFunction = function;
    builder->basicBlock = basicBlock;
    builder->allocaBuilder = allocaBuilder;
    builder->lexicalEnvironment = lexicalEnvironment;
    return builder;
}

CRANK_TALK_API crank_talk_HIRBuilder_t *
crank_talk_HIRBuilder_copyWithBasicBlock(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRBasicBlock_t *basicBlock)
{
    crank_talk_HIRBuilder_t *newBuilder = (crank_talk_HIRBuilder_t *)CrankAllocateValuesObjectWithClassName(context, HIRBuilder);
    newBuilder->hirFunction = builder->hirFunction;
    newBuilder->basicBlock = basicBlock;
    newBuilder->allocaBuilder = builder->allocaBuilder;
    newBuilder->lexicalEnvironment = builder->lexicalEnvironment;
    return newBuilder;
}

CRANK_TALK_API void
crank_talk_HIRBuilder_addInstruction(crank_talk_HIRBuilder_t *builder, crank_talk_HIRInstruction_t *instruction)
{
    crank_talk_HIRBasicBlock_addInstruction(builder->basicBlock, instruction);
}

CRANK_TALK_API bool
crank_talk_HIRBuilder_isLastTerminator(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder)
{
    if(!builder->basicBlock)
        return true;

    if(!builder->basicBlock->lastInstruction)
        return false;

    return crank_talk_oop_asBoolean(context, crank_talk_send0(context, context->roots.isTerminatorSymbol, (crank_talk_oop_t)builder->basicBlock->lastInstruction));
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_alloca(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRAllocaInstruction);
    crank_talk_HIRAllocaInstruction_t *instruction = (crank_talk_HIRAllocaInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_branch(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRBasicBlock_t *destination, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRBranchInstruction);
    crank_talk_HIRBranchInstruction_t *branchInstruction = (crank_talk_HIRBranchInstruction_t*)instructionValue;
    branchInstruction->super.super.super.sourcePosition = sourcePosition;
    branchInstruction->destination = destination;
    crank_talk_HIRBuilder_addInstruction(builder, &branchInstruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_conditionalBranch(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *condition, crank_talk_HIRBasicBlock_t *trueDestination, crank_talk_HIRBasicBlock_t *falseDestination, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRConditionalBranchInstruction);
    crank_talk_HIRConditionalBranchInstruction_t *branchInstruction = (crank_talk_HIRConditionalBranchInstruction_t*)instructionValue;
    branchInstruction->super.super.super.sourcePosition = sourcePosition;
    branchInstruction->condition = condition;
    branchInstruction->trueDestination = trueDestination;
    branchInstruction->falseDestination = falseDestination;
    crank_talk_HIRBuilder_addInstruction(builder, &branchInstruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_makeClosure(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_CompiledFunction_t *function, crank_talk_Array_t *capturedValues, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRMakeClosureInstruction);
    crank_talk_HIRMakeClosureInstruction_t *instruction = (crank_talk_HIRMakeClosureInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->function = function;
    instruction->capturedValues = capturedValues;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_HIRPhiInstruction_t *
crank_talk_HIRBuilder_phi(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRPhiInstruction);
    crank_talk_HIRPhiInstruction_t *instruction = (crank_talk_HIRPhiInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instruction;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_phiSource(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRPhiInstruction_t *targetPhi, crank_talk_HIRValue_t *value, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRPhiSourceInstruction);
    crank_talk_HIRPhiSourceInstruction_t *instruction = (crank_talk_HIRPhiSourceInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->targetPhi = targetPhi;
    instruction->value = value;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_nonLocalReturn(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *resultValue, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRNonLocalReturnInstruction);
    crank_talk_HIRNonLocalReturnInstruction_t *instruction = (crank_talk_HIRNonLocalReturnInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->resultValue = resultValue;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_return(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *resultValue, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRReturnInstruction);
    crank_talk_HIRReturnInstruction_t *instruction = (crank_talk_HIRReturnInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->resultValue = resultValue;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_send(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *selector, crank_talk_Array_t *receiverWithArguments, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRSendInstruction);
    crank_talk_HIRSendInstruction_t *instruction = (crank_talk_HIRSendInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->selector = selector;
    instruction->receiverWithArguments = receiverWithArguments;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_superSend(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *lookupClass, crank_talk_HIRValue_t *selector, crank_talk_Array_t *receiverWithArguments, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRSuperSendInstruction);
    crank_talk_HIRSuperSendInstruction_t *instruction = (crank_talk_HIRSuperSendInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->lookupClass = lookupClass;
    instruction->selector = selector;
    instruction->receiverWithArguments = receiverWithArguments;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_slotLoad(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *receiver, size_t slotIndex, crank_talk_oop_t sourcePosition)
{
    assert(slotIndex >= 1);
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRSlotLoadInstruction);
    crank_talk_HIRSlotLoadInstruction_t* instruction = (crank_talk_HIRSlotLoadInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->receiver = receiver;
    instruction->slotIndex = crank_talk_oop_encodeSmallInteger(slotIndex);
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_slotStore(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *receiver, size_t slotIndex, crank_talk_HIRValue_t *value, crank_talk_oop_t sourcePosition)
{
    assert(slotIndex >= 1);
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRSlotStoreInstruction);
    crank_talk_HIRSlotStoreInstruction_t* instruction = (crank_talk_HIRSlotStoreInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->receiver = receiver;
    instruction->slotIndex = crank_talk_oop_encodeSmallInteger(slotIndex);
    instruction->value = value;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_thisContext(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRThisContextInstruction);
    crank_talk_HIRThisContextInstruction_t *instruction = (crank_talk_HIRThisContextInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;

}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_unreachable(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRUnreachableInstruction);
    crank_talk_HIRUnreachableInstruction_t *instruction = (crank_talk_HIRUnreachableInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_variableLoad(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *variable, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRVariableLoadInstruction);
    crank_talk_HIRVariableLoadInstruction_t *instruction = (crank_talk_HIRVariableLoadInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->variable = variable;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_HIRBuilder_variableStore(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *variable, crank_talk_HIRValue_t *value, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRVariableStoreInstruction);
    crank_talk_HIRVariableStoreInstruction_t *instruction = (crank_talk_HIRVariableStoreInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    instruction->variable = variable;
    instruction->value = value;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

CRANK_TALK_API uint16_t
crank_talk_HIRValue_getBytecodeAssemblyOperandWith(crank_talk_VMContext_t *context, crank_talk_HIRValue_t *value, crank_talk_oop_t bytecodeAssembler)
{
    crank_talk_oop_t operand = crank_talk_send1(context, context->roots.getBytecodeAssemblyOperandWithSymbol, (crank_talk_oop_t)value, bytecodeAssembler);
    return (uint16_t)crank_talk_oop_decodeSmallInteger(operand);
}

CRANK_TALK_API void
crank_talk_HIRInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_HIRInstruction_t *instruction, crank_talk_oop_t bytecodeAssembler)
{
    crank_talk_send1(context, context->roots.emitBytecodeAssemblyWithSymbol, (crank_talk_oop_t)instruction, bytecodeAssembler);
}

static void
crank_talk_HIRConstantLiteralValue_printOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];

    crank_talk_HIRConstantLiteralValue_t *constantLiteral = (crank_talk_HIRConstantLiteralValue_t*)receiver;

    crank_talk_StringStream_insertCString(primContext->context, stream, "constantLiteral ");
    crank_talk_StringStream_print(primContext->context, stream, constantLiteral->value);

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRFunctionLocal_printOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];

    crank_talk_HIRFunctionLocal_t *functionLocal = (crank_talk_HIRFunctionLocal_t*)receiver;

    crank_talk_StringStream_insertCString(primContext->context, stream, "$");
    crank_talk_StringStream_print(primContext->context, stream, functionLocal->index);
    if(functionLocal->name)
    {
        crank_talk_StringStream_insertCString(primContext->context, stream, "|");
        crank_talk_StringStream_insertBytesObject(primContext->context, stream, (crank_talk_oop_t)functionLocal->name);
    }


    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRAllocaInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    
    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_StringStream_print(primContext->context, stream, receiver);
    crank_talk_StringStream_insertCString(primContext->context, stream, " := alloca");

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRBranchInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_HIRBranchInstruction_t *instruction = (crank_talk_HIRBranchInstruction_t*)receiver;

    crank_talk_StringStream_insertCString(primContext->context, stream, "branch destination ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->destination);

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRConditionalBranchInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_HIRConditionalBranchInstruction_t *instruction = (crank_talk_HIRConditionalBranchInstruction_t*)receiver;

    crank_talk_StringStream_insertCString(primContext->context, stream, "condition ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->condition);
    crank_talk_StringStream_insertCString(primContext->context, stream, " trueDestination ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->trueDestination);
    crank_talk_StringStream_insertCString(primContext->context, stream, " falseDestination ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->falseDestination);

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRMakeClosureInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_HIRMakeClosureInstruction_t *instruction = (crank_talk_HIRMakeClosureInstruction_t*)receiver;

    crank_talk_StringStream_print(primContext->context, stream, receiver);
    crank_talk_StringStream_insertCString(primContext->context, stream, " := makeClosure ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->function);
    size_t captureCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)instruction->capturedValues);
    for(size_t i = 0 ; i < captureCount; ++i)
    {
        crank_talk_StringStream_insertCString(primContext->context, stream, ", ");
        crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->capturedValues->elements[i]);
    }

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRPhiInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_StringStream_print(primContext->context, stream, receiver);
    crank_talk_StringStream_insertCString(primContext->context, stream, " := phi");

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRPhiSourceInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_HIRPhiSourceInstruction_t *instruction = (crank_talk_HIRPhiSourceInstruction_t*)receiver;

    crank_talk_StringStream_insertCString(primContext->context, stream, "phi ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->targetPhi);
    crank_talk_StringStream_insertCString(primContext->context, stream, " source ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->value);

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRReturnInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_HIRReturnInstruction_t *instruction = (crank_talk_HIRReturnInstruction_t*)receiver;

    crank_talk_StringStream_insertCString(primContext->context, stream, "return ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->resultValue);

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRSendInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_HIRSendInstruction_t *instruction = (crank_talk_HIRSendInstruction_t*)receiver;

    crank_talk_StringStream_print(primContext->context, stream, receiver);
    crank_talk_StringStream_insertCString(primContext->context, stream, " := send ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->selector);
    size_t messageArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)instruction->receiverWithArguments);
    for(size_t i = 0 ; i < messageArgumentCount; ++i)
    {
        crank_talk_StringStream_insertCString(primContext->context, stream, ", ");
        crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->receiverWithArguments->elements[i]);
    }

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRSuperSendInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_HIRSuperSendInstruction_t *instruction = (crank_talk_HIRSuperSendInstruction_t*)receiver;

    crank_talk_StringStream_print(primContext->context, stream, receiver);
    crank_talk_StringStream_insertCString(primContext->context, stream, " := super ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->lookupClass);
    crank_talk_StringStream_insertCString(primContext->context, stream, " send ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->selector);
    size_t messageArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)instruction->receiverWithArguments);
    for(size_t i = 0 ; i < messageArgumentCount; ++i)
    {
        crank_talk_StringStream_insertCString(primContext->context, stream, ", ");
        crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->receiverWithArguments->elements[i]);
    }

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRSlotLoadInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];

    crank_talk_HIRSlotLoadInstruction_t *instruction = (crank_talk_HIRSlotLoadInstruction_t*)receiver;

    crank_talk_StringStream_print(primContext->context, stream, receiver);
    crank_talk_StringStream_insertCString(primContext->context, stream, " := from: ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->receiver);

    crank_talk_StringStream_insertCString(primContext->context, stream, " loadSlot: ");
    crank_talk_StringStream_print(primContext->context, stream, instruction->slotIndex);

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRSlotStoreInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];

    crank_talk_HIRSlotStoreInstruction_t *instruction = (crank_talk_HIRSlotStoreInstruction_t*)receiver;

    crank_talk_StringStream_insertCString(primContext->context, stream, "from: ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->receiver);

    crank_talk_StringStream_insertCString(primContext->context, stream, " slot: ");
    crank_talk_StringStream_print(primContext->context, stream, instruction->slotIndex);

    crank_talk_StringStream_insertCString(primContext->context, stream, " store: ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->value);

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRThisContextInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_StringStream_print(primContext->context, stream, receiver);
    crank_talk_StringStream_insertCString(primContext->context, stream, " := thisContext");

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRUnreachableInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_StringStream_insertCString(primContext->context, stream, "unreachable");

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRVariableLoadInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    
    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_HIRVariableLoadInstruction_t *instruction = (crank_talk_HIRVariableLoadInstruction_t *)receiver;
    crank_talk_StringStream_print(primContext->context, stream, receiver);
    crank_talk_StringStream_insertCString(primContext->context, stream, " := variableLoad ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->variable);
    
    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRVariableStoreInstruction_fullPrintOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    
    crank_talk_oop_t receiver = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];
    
    crank_talk_HIRVariableStoreInstruction_t *instruction = (crank_talk_HIRVariableStoreInstruction_t *)receiver;
    crank_talk_StringStream_insertCString(primContext->context, stream, "variable: ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->variable);
    crank_talk_StringStream_insertCString(primContext->context, stream, " store: ");
    crank_talk_StringStream_print(primContext->context, stream, (crank_talk_oop_t)instruction->value);

    return crank_talk_Primitive_return(primContext, receiver);
}

static void
crank_talk_HIRInstruction_isTerminator(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, false));
}

static void
crank_talk_HIRBranchInstruction_isTerminator(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRConditionalBranchInstruction_isTerminator(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRNonLocalReturnInstruction_isTerminator(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRReturnInstruction_isTerminator(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRUnreachableInstruction_isTerminator(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRAllocaInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRBranchInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, false));
}

static void
crank_talk_HIRConditionalBranchInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, false));
}

static void
crank_talk_HIRMakeClosureInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRPhiInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRPhiSourceInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, false));
}

static void
crank_talk_HIRNonLocalReturnInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, false));
}

static void
crank_talk_HIRReturnInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, false));
}

static void
crank_talk_HIRSendInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRSuperSendInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRSlotLoadInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}


static void
crank_talk_HIRSlotStoreInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, false));
}

static void
crank_talk_HIRThisContextInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRUnreachableInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, false));
}

static void
crank_talk_HIRVariableLoadInstruction_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, true));
}

static void
crank_talk_HIRVariableStoreInstructionClass_hasResultValue(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, false));
}

static crank_talk_oop_t
crank_talk_HIRValue_isHIRVariable(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    return crank_talk_oop_fromBoolean(context, false);
}

static crank_talk_oop_t
crank_talk_HIRAllocaInstruction_isHIRVariable(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    return crank_talk_oop_fromBoolean(context, true);
}

static crank_talk_oop_t
crank_talk_HIRCapture_isHIRVariable(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    crank_talk_HIRCapture_t *self = (crank_talk_HIRCapture_t *)arguments[0];
    return self->isVariable;
}

static crank_talk_oop_t
crank_talk_HIRConstantLiteralValue_getBytecodeAssemblyOperandWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_HIRConstantLiteralValue_t *literal = (crank_talk_HIRConstantLiteralValue_t*)arguments[0];
    crank_talk_BytecodeAssembler_t *assembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    return crank_talk_oop_encodeSmallInteger(crank_talk_BytecodeAssembler_getOrCreateLiteral(context, assembler, literal->value));
}

static crank_talk_oop_t
crank_talk_HIRFunctionLocal_getBytecodeAssemblyOperandWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_HIRFunctionLocal_t *functionLocal = (crank_talk_HIRFunctionLocal_t*)arguments[0];
    return functionLocal->bytecodeVectorIndex;
}

static crank_talk_oop_t
crank_talk_HIRAllocaInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRAllocaInstruction_t *allocaInstruction = (crank_talk_HIRAllocaInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeAssembler_alloca(context, bytecodeAssembler, (crank_talk_BytecodeVectorOperand_t)crank_talk_oop_decodeSmallInteger(allocaInstruction->super.super.bytecodeVectorIndex), allocaInstruction->super.super.super.sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRBranchInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRBranchInstruction_t *branchInstruction = (crank_talk_HIRBranchInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeAssembler_jump(context, bytecodeAssembler, (crank_talk_BytecodeLabel_t*)branchInstruction->destination->bytecodeLabel, branchInstruction->super.super.super.sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRConditionalBranchInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRConditionalBranchInstruction_t *branchInstruction = (crank_talk_HIRConditionalBranchInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeVectorOperand_t condition = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, branchInstruction->condition, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeAssembler_jumpIfTrue(context, bytecodeAssembler, condition, (crank_talk_BytecodeLabel_t*)branchInstruction->trueDestination->bytecodeLabel, branchInstruction->super.super.super.sourcePosition);
    crank_talk_BytecodeAssembler_jumpIfFalse(context, bytecodeAssembler, condition, (crank_talk_BytecodeLabel_t*)branchInstruction->falseDestination->bytecodeLabel, branchInstruction->super.super.super.sourcePosition);
    crank_talk_BytecodeAssembler_booleanExpected(context, bytecodeAssembler, branchInstruction->super.super.super.sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRMakeClosureInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRMakeClosureInstruction_t *instruction = (crank_talk_HIRMakeClosureInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    size_t captureCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)instruction->capturedValues);
    crank_talk_BytecodeVectorOperand_t functionOperand = crank_talk_BytecodeAssembler_getOrCreateLiteral(context, bytecodeAssembler, (crank_talk_oop_t)instruction->function);

    crank_talk_BytecodeAssembler_beginClosure(context, bytecodeAssembler, functionOperand, captureCount, instruction->super.super.super.sourcePosition);
    for(size_t i = 0; i < captureCount; ++i)
    {
        crank_talk_BytecodeVectorOperand_t capturedValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, (crank_talk_HIRValue_t*)instruction->capturedValues->elements[i], (crank_talk_oop_t)bytecodeAssembler);
        crank_talk_BytecodeAssembler_pushClosureCapture(context, bytecodeAssembler, capturedValue, instruction->super.super.super.sourcePosition);
    }

    crank_talk_BytecodeAssembler_finishClosure(context, bytecodeAssembler, (crank_talk_BytecodeVectorOperand_t)crank_talk_oop_decodeSmallInteger(instruction->super.super.bytecodeVectorIndex), instruction->super.super.super.sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRPhiInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];

    // Nothing is required here.
    return self;
}

static crank_talk_oop_t
crank_talk_HIRPhiSourceInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRPhiSourceInstruction_t *phiSourceInstruction = (crank_talk_HIRPhiSourceInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    // TODO: Move onto the target phi.
    crank_talk_BytecodeVectorOperand_t sourceValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, phiSourceInstruction->value, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeVectorOperand_t targetPhi = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, &phiSourceInstruction->targetPhi->super.super.super, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeAssembler_move(context, bytecodeAssembler, sourceValue, targetPhi, phiSourceInstruction->super.super.super.sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRSendInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRSendInstruction_t *sendInstruction = (crank_talk_HIRSendInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    // Some messages are optimized.
    size_t messageArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendInstruction->receiverWithArguments);
    if(crank_talk_oop_isKindOf(context, (crank_talk_oop_t)sendInstruction->selector, context->roots.HIRConstantLiteralValueClass))
    {
        // TODO: Refactor those cases in a dictionary.
        crank_talk_HIRConstantLiteralValue_t *selectorLiteralValue = (crank_talk_HIRConstantLiteralValue_t *)sendInstruction->selector;
        if(selectorLiteralValue->value == context->roots.classSymbol && messageArgumentCount == 1)
        {
            uint16_t operand = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, (crank_talk_HIRValue_t*)sendInstruction->receiverWithArguments->elements[0], (crank_talk_oop_t)bytecodeAssembler);  
            uint16_t result = (uint16_t)crank_talk_oop_decodeSmallInteger(sendInstruction->super.super.bytecodeVectorIndex);
            crank_talk_BytecodeAssembler_class(context, bytecodeAssembler, result, operand, sendInstruction->super.super.super.sourcePosition);
            return self;
        }
        else if(selectorLiteralValue->value == context->roots.notSymbol && messageArgumentCount == 1)
        {
            uint16_t operand = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, (crank_talk_HIRValue_t*)sendInstruction->receiverWithArguments->elements[0], (crank_talk_oop_t)bytecodeAssembler);  
            uint16_t result = (uint16_t)crank_talk_oop_decodeSmallInteger(sendInstruction->super.super.bytecodeVectorIndex);
            crank_talk_BytecodeAssembler_not(context, bytecodeAssembler, result, operand, sendInstruction->super.super.super.sourcePosition);
            return self;
        }
        else if(selectorLiteralValue->value == context->roots.identityEqualsSymbol && messageArgumentCount == 2)
        {
            uint16_t leftOperand = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, (crank_talk_HIRValue_t*)sendInstruction->receiverWithArguments->elements[0], (crank_talk_oop_t)bytecodeAssembler);
            uint16_t rightOperand = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, (crank_talk_HIRValue_t*)sendInstruction->receiverWithArguments->elements[1], (crank_talk_oop_t)bytecodeAssembler);
            uint16_t result = (uint16_t)crank_talk_oop_decodeSmallInteger(sendInstruction->super.super.bytecodeVectorIndex);
            crank_talk_BytecodeAssembler_identityEquals(context, bytecodeAssembler, result, leftOperand, rightOperand, sendInstruction->super.super.super.sourcePosition);
            return self;
        }
        else if(selectorLiteralValue->value == context->roots.identityNotEqualsSymbol && messageArgumentCount == 2)
        {
            uint16_t leftOperand = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, (crank_talk_HIRValue_t*)sendInstruction->receiverWithArguments->elements[0], (crank_talk_oop_t)bytecodeAssembler);
            uint16_t rightOperand = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, (crank_talk_HIRValue_t*)sendInstruction->receiverWithArguments->elements[1], (crank_talk_oop_t)bytecodeAssembler);
            uint16_t result = (uint16_t)crank_talk_oop_decodeSmallInteger(sendInstruction->super.super.bytecodeVectorIndex);
            crank_talk_BytecodeAssembler_identityNotEquals(context, bytecodeAssembler, result, leftOperand, rightOperand, sendInstruction->super.super.super.sourcePosition);
            return self;
        }
    }

    crank_talk_BytecodeAssembler_beginSend(context, bytecodeAssembler, sendInstruction->super.super.super.sourcePosition);
    for(size_t i = 0; i < messageArgumentCount; ++i)
    {
        crank_talk_BytecodeVectorOperand_t argumentValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, (crank_talk_HIRValue_t*)sendInstruction->receiverWithArguments->elements[i], (crank_talk_oop_t)bytecodeAssembler);  
        crank_talk_BytecodeAssembler_callArgument(context, bytecodeAssembler, argumentValue, sendInstruction->super.super.super.sourcePosition);
    }

    crank_talk_BytecodeVectorOperand_t selectorValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, sendInstruction->selector, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeAssembler_sendMessage(context, bytecodeAssembler, (crank_talk_BytecodeVectorOperand_t)crank_talk_oop_decodeSmallInteger(sendInstruction->super.super.bytecodeVectorIndex), selectorValue, sendInstruction->super.super.super.sourcePosition);

    return self;
}

static crank_talk_oop_t
crank_talk_HIRSuperSendInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRSuperSendInstruction_t *sendInstruction = (crank_talk_HIRSuperSendInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    size_t messageArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendInstruction->receiverWithArguments);
    crank_talk_BytecodeAssembler_beginSend(context, bytecodeAssembler, sendInstruction->super.super.super.sourcePosition);
    for(size_t i = 0; i < messageArgumentCount; ++i)
    {
        crank_talk_BytecodeVectorOperand_t argumentValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, (crank_talk_HIRValue_t*)sendInstruction->receiverWithArguments->elements[i], (crank_talk_oop_t)bytecodeAssembler);  
        crank_talk_BytecodeAssembler_callArgument(context, bytecodeAssembler, argumentValue, sendInstruction->super.super.super.sourcePosition);
    }

    crank_talk_BytecodeVectorOperand_t lookupClassValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, sendInstruction->lookupClass, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeVectorOperand_t selectorValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, sendInstruction->selector, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeAssembler_superSendMessage(context, bytecodeAssembler, (crank_talk_BytecodeVectorOperand_t)crank_talk_oop_decodeSmallInteger(sendInstruction->super.super.bytecodeVectorIndex), lookupClassValue, selectorValue, sendInstruction->super.super.super.sourcePosition);

    return self;
}

static crank_talk_oop_t
crank_talk_HIRSlotLoadInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRSlotLoadInstruction_t *slotInstruction = (crank_talk_HIRSlotLoadInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeVectorOperand_t receiverValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, slotInstruction->receiver, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeVectorOperand_t resultValue = (crank_talk_BytecodeVectorOperand_t)crank_talk_oop_decodeSmallInteger(slotInstruction->super.super.bytecodeVectorIndex);

    crank_talk_BytecodeAssembler_loadSlot(context, bytecodeAssembler, resultValue, receiverValue, (crank_talk_BytecodeVectorOperand_t)crank_talk_oop_decodeSmallInteger(slotInstruction->slotIndex), slotInstruction->super.super.super.sourcePosition);

    return self;
}

static crank_talk_oop_t
crank_talk_HIRSlotStoreInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRSlotStoreInstruction_t *slotInstruction = (crank_talk_HIRSlotStoreInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeVectorOperand_t receiverValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, slotInstruction->receiver, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeVectorOperand_t assignedValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, slotInstruction->value, (crank_talk_oop_t)bytecodeAssembler);

    crank_talk_BytecodeAssembler_storeSlot(context, bytecodeAssembler, receiverValue, crank_talk_oop_decodeSmallInteger(slotInstruction->slotIndex), assignedValue, slotInstruction->value->sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRThisContextInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRThisContextInstruction_t *instruction = (crank_talk_HIRThisContextInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeVectorOperand_t resultValue = (crank_talk_BytecodeVectorOperand_t)crank_talk_oop_decodeSmallInteger(instruction->super.super.bytecodeVectorIndex);
    crank_talk_BytecodeAssembler_thisContext(context, bytecodeAssembler, resultValue, instruction->super.super.super.sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRNonLocalReturnInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRReturnInstruction_t *returnInstruction = (crank_talk_HIRReturnInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeVectorOperand_t resultValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, returnInstruction->resultValue, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeAssembler_nonLocalReturn(context, bytecodeAssembler, resultValue, returnInstruction->super.super.super.sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRReturnInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRReturnInstruction_t *returnInstruction = (crank_talk_HIRReturnInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeVectorOperand_t resultValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, returnInstruction->resultValue, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeAssembler_return(context, bytecodeAssembler, resultValue, returnInstruction->super.super.super.sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRVariableLoadInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRVariableLoadInstruction_t *loadInstruction = (crank_talk_HIRVariableLoadInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeVectorOperand_t variableValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, loadInstruction->variable, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeAssembler_loadFromVariable(context, bytecodeAssembler, (crank_talk_BytecodeVectorOperand_t)crank_talk_oop_decodeSmallInteger(loadInstruction->super.super.bytecodeVectorIndex), variableValue, loadInstruction->super.super.super.sourcePosition);
    return self;
}

static crank_talk_oop_t
crank_talk_HIRVariableStoreInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount == 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_HIRVariableStoreInstruction_t *storeInstruction = (crank_talk_HIRVariableStoreInstruction_t*)self;
    crank_talk_BytecodeAssembler_t *bytecodeAssembler = (crank_talk_BytecodeAssembler_t *)arguments[1];

    crank_talk_BytecodeVectorOperand_t variableValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, storeInstruction->variable, (crank_talk_oop_t)bytecodeAssembler);
    crank_talk_BytecodeVectorOperand_t valueValue = crank_talk_HIRValue_getBytecodeAssemblyOperandWith(context, storeInstruction->value, (crank_talk_oop_t)bytecodeAssembler);

    crank_talk_BytecodeAssembler_storeInVariable(context, bytecodeAssembler, valueValue, variableValue, storeInstruction->super.super.super.sourcePosition);
    return self;
}

void
crank_talk_registerHIRPrimitives(crank_talk_VMContext_t *context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRConstantLiteralValueClass, "printOn:", 2, "HIRConstantLiteralValue::printOn:", crank_talk_HIRConstantLiteralValue_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRFunctionLocalClass, "printOn:", 2, "HIRFunctionLocal::printOn:", crank_talk_HIRFunctionLocal_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRAllocaInstructionClass, "fullPrintOn:", 2, "HIRAllocaInstruction::fullPrintOn:", crank_talk_HIRAllocaInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRBranchInstructionClass, "fullPrintOn:", 2, "HIRBranchInstruction::fullPrintOn:", crank_talk_HIRBranchInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRConditionalBranchInstructionClass, "fullPrintOn:", 2, "HIRConditionalBranchInstruction::fullPrintOn:", crank_talk_HIRConditionalBranchInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRMakeClosureInstructionClass, "fullPrintOn:", 2, "HIRMakeClosureInstruction::fullPrintOn:", crank_talk_HIRMakeClosureInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRPhiInstructionClass, "fullPrintOn:", 2, "HIRPhiInstruction::fullPrintOn:", crank_talk_HIRPhiInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRPhiSourceInstructionClass, "fullPrintOn:", 2, "HIRPhiSourceInstruction::fullPrintOn:", crank_talk_HIRPhiSourceInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRReturnInstructionClass, "fullPrintOn:", 2, "HIRReturnInstruction::fullPrintOn:", crank_talk_HIRReturnInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRSendInstructionClass, "fullPrintOn:", 2, "HIRSendInstruction::fullPrintOn:", crank_talk_HIRSendInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRSuperSendInstructionClass, "fullPrintOn:", 2, "HIRSuperSendInstruction::fullPrintOn:", crank_talk_HIRSuperSendInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRSlotLoadInstructionClass, "fullPrintOn:", 2, "HIRSlotLoadInstruction::fullPrintOn:", crank_talk_HIRSlotLoadInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRSlotStoreInstructionClass, "fullPrintOn:", 2, "HIRSlotStoreInstruction::fullPrintOn:", crank_talk_HIRSlotStoreInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRThisContextInstructionClass, "fullPrintOn:", 2, "HIRThisContextInstruction::fullPrintOn:", crank_talk_HIRThisContextInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRUnreachableInstructionClass, "fullPrintOn:", 2, "HIRUnreachableInstruction::fullPrintOn:", crank_talk_HIRUnreachableInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRVariableLoadInstructionClass, "fullPrintOn:", 2, "HIRVariableLoadInstruction::fullPrintOn:", crank_talk_HIRVariableLoadInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRVariableStoreInstructionClass, "fullPrintOn:", 2, "HIRVariableStoreInstruction::fullPrintOn:", crank_talk_HIRVariableStoreInstruction_fullPrintOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRInstructionClass, "isTerminator", 1, "HIRInstruction::isTerminator", crank_talk_HIRInstruction_isTerminator);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRBranchInstructionClass, "isTerminator", 1, "HIRBranchInstruction::isTerminator", crank_talk_HIRBranchInstruction_isTerminator);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRConditionalBranchInstructionClass, "isTerminator", 1, "HIRConditionalBranchInstruction::isTerminator", crank_talk_HIRConditionalBranchInstruction_isTerminator);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRNonLocalReturnInstructionClass, "isTerminator", 1, "HIRNonLocalReturnInstruction::isTerminator", crank_talk_HIRNonLocalReturnInstruction_isTerminator);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRReturnInstructionClass, "isTerminator", 1, "HIRReturnInstruction::isTerminator", crank_talk_HIRReturnInstruction_isTerminator);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRUnreachableInstructionClass, "isTerminator", 1, "HIRUnreachableInstruction::isTerminator", crank_talk_HIRUnreachableInstruction_isTerminator);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRAllocaInstructionClass, "hasResultValue", 1, "HIRAllocaInstruction::hasResultValue", crank_talk_HIRAllocaInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRBranchInstructionClass, "hasResultValue", 1, "HIRBranchInstruction::hasResultValue", crank_talk_HIRBranchInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRConditionalBranchInstructionClass, "hasResultValue", 1, "HIRConditionalBranchInstruction::hasResultValue", crank_talk_HIRConditionalBranchInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRMakeClosureInstructionClass, "hasResultValue", 1, "HIRMakeClosureInstruction::hasResultValue", crank_talk_HIRMakeClosureInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRPhiInstructionClass, "hasResultValue", 1, "HIRPhiInstruction::hasResultValue", crank_talk_HIRPhiInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRPhiSourceInstructionClass, "hasResultValue", 1, "HIRPhiSourceInstruction::hasResultValue", crank_talk_HIRPhiSourceInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRNonLocalReturnInstructionClass, "hasResultValue", 1, "HIRNonLocalReturnInstruction::hasResultValue", crank_talk_HIRNonLocalReturnInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRReturnInstructionClass, "hasResultValue", 1, "HIRReturnInstruction::hasResultValue", crank_talk_HIRReturnInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRSendInstructionClass, "hasResultValue", 1, "HIRSendInstruction::hasResultValue", crank_talk_HIRSendInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRSuperSendInstructionClass, "hasResultValue", 1, "HIRSuperSendInstruction::hasResultValue", crank_talk_HIRSuperSendInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRSlotLoadInstructionClass, "hasResultValue", 1, "HIRSlotLoadInstruction::hasResultValue", crank_talk_HIRSlotLoadInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRSlotStoreInstructionClass, "hasResultValue", 1, "HIRSlotStoreInstruction::hasResultValue", crank_talk_HIRSlotStoreInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRThisContextInstructionClass, "hasResultValue", 1, "HIRThisContextInstruction::hasResultValue", crank_talk_HIRThisContextInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRUnreachableInstructionClass, "hasResultValue", 1, "HIRUnreachableInstruction::hasResultValue", crank_talk_HIRUnreachableInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRVariableLoadInstructionClass, "hasResultValue", 1, "HIRVariableLoadInstruction::hasResultValue", crank_talk_HIRVariableLoadInstruction_hasResultValue);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRVariableStoreInstructionClass, "hasResultValue", 1, "HIRVariableStoreInstruction::hasResultValue", crank_talk_HIRVariableStoreInstructionClass_hasResultValue);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRValueClass, "isHIRVariable", 1, crank_talk_HIRValue_isHIRVariable);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRAllocaInstructionClass, "isHIRVariable", 1, crank_talk_HIRAllocaInstruction_isHIRVariable);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRCaptureClass, "isHIRVariable", 1, crank_talk_HIRCapture_isHIRVariable);


    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRConstantLiteralValueClass, "getBytecodeAssemblyOperandWith:", 2, crank_talk_HIRConstantLiteralValue_getBytecodeAssemblyOperandWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRFunctionLocalClass, "getBytecodeAssemblyOperandWith:", 2, crank_talk_HIRFunctionLocal_getBytecodeAssemblyOperandWith);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRAllocaInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRAllocaInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRBranchInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRBranchInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRConditionalBranchInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRConditionalBranchInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRMakeClosureInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRMakeClosureInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRPhiInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRPhiInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRPhiSourceInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRPhiSourceInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRNonLocalReturnInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRNonLocalReturnInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRReturnInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRReturnInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRSendInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRSendInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRSuperSendInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRSuperSendInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRSlotLoadInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRSlotLoadInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRSlotStoreInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRSlotStoreInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRThisContextInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRThisContextInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRVariableLoadInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRVariableLoadInstruction_emitBytecodeAssemblyWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.HIRVariableStoreInstructionClass, "emitBytecodeAssemblyWith:", 2, crank_talk_HIRVariableStoreInstruction_emitBytecodeAssemblyWith);
}
