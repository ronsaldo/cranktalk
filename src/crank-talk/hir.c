#include "crank-talk/hir.h"
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
    return builder;
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
crank_talk_HIRBuilder_unreachable(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_oop_t sourcePosition)
{
    crank_talk_oop_t instructionValue = CrankAllocateValuesObjectWithClassName(context, HIRUnreachableInstruction);
    crank_talk_HIRUnreachableInstruction_t *instruction = (crank_talk_HIRUnreachableInstruction_t*)instructionValue;
    instruction->super.super.super.sourcePosition = sourcePosition;
    crank_talk_HIRBuilder_addInstruction(builder, &instruction->super);
    return instructionValue;
}

static crank_talk_oop_t
crank_talk_HIRConstantLiteralValue_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];

    crank_talk_HIRConstantLiteralValue_t *constantLiteral = (crank_talk_HIRConstantLiteralValue_t*)receiver;

    crank_talk_StringStream_insertCString(context, stream, "constantLiteral ");
    crank_talk_StringStream_print(context, stream, constantLiteral->value);

    return receiver;
}

static crank_talk_oop_t
crank_talk_HIRFunctionLocal_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];

    crank_talk_HIRFunctionLocal_t *functionLocal = (crank_talk_HIRFunctionLocal_t*)receiver;

    crank_talk_StringStream_insertCString(context, stream, "$");
    crank_talk_StringStream_print(context, stream, functionLocal->index);
    if(functionLocal->name)
    {
        crank_talk_StringStream_insertCString(context, stream, "|");
        crank_talk_StringStream_insertBytesObject(context, stream, (crank_talk_oop_t)functionLocal->name);
    }


    return receiver;
}

static crank_talk_oop_t
crank_talk_HIRAllocaInstruction_fullPrintOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_StringStream_print(context, stream, receiver);
    crank_talk_StringStream_insertCString(context, stream, " := alloca");

    return receiver;
}

static crank_talk_oop_t
crank_talk_HIRBranchInstruction_fullPrintOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_HIRBranchInstruction_t *instruction = (crank_talk_HIRBranchInstruction_t*)receiver;

    crank_talk_StringStream_insertCString(context, stream, "destination ");
    crank_talk_StringStream_print(context, stream, (crank_talk_oop_t)instruction->destination);

    return receiver;
}

static crank_talk_oop_t
crank_talk_HIRReturnInstruction_fullPrintOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_HIRReturnInstruction_t *instruction = (crank_talk_HIRReturnInstruction_t*)receiver;

    crank_talk_StringStream_insertCString(context, stream, "return ");
    crank_talk_StringStream_print(context, stream, (crank_talk_oop_t)instruction->resultValue);

    return receiver;
}

static crank_talk_oop_t
crank_talk_HIRUnreachableInstruction_fullPrintOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    
    crank_talk_StringStream_insertCString(context, stream, "unreachable");

    return receiver;
}

static crank_talk_oop_t
crank_talk_HIRInstruction_isTerminator(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    return crank_talk_oop_fromBoolean(context, false);
}

static crank_talk_oop_t
crank_talk_HIRBranchInstruction_isTerminator(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    return crank_talk_oop_fromBoolean(context, true);
}

static crank_talk_oop_t
crank_talk_HIRReturnInstruction_isTerminator(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    return crank_talk_oop_fromBoolean(context, true);
}

static crank_talk_oop_t
crank_talk_HIRUnreachableInstruction_isTerminator(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    return crank_talk_oop_fromBoolean(context, true);
}

void
crank_talk_registerHIRPrimitives(crank_talk_VMContext_t *context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRConstantLiteralValueClass, "printOn:", 2, crank_talk_HIRConstantLiteralValue_printOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRFunctionLocalClass, "printOn:", 2, crank_talk_HIRFunctionLocal_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRAllocaInstructionClass, "fullPrintOn:", 2, crank_talk_HIRAllocaInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRBranchInstructionClass, "fullPrintOn:", 2, crank_talk_HIRBranchInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRReturnInstructionClass, "fullPrintOn:", 2, crank_talk_HIRReturnInstruction_fullPrintOn_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRUnreachableInstructionClass, "fullPrintOn:", 2, crank_talk_HIRUnreachableInstruction_fullPrintOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRInstructionClass, "isTerminator", 1, crank_talk_HIRInstruction_isTerminator);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRBranchInstructionClass, "isTerminator", 1, crank_talk_HIRBranchInstruction_isTerminator);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRReturnInstructionClass, "isTerminator", 1, crank_talk_HIRReturnInstruction_isTerminator);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRUnreachableInstructionClass, "isTerminator", 1, crank_talk_HIRUnreachableInstruction_isTerminator);
}