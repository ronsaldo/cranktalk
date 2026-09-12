#include "crank-talk/bytecode.h"
#include "crank-talk/bytes.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/gc.h"
#include "crank-talk/message.h"
#include "crank-talk/ordered-collection.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

const char *CrankTalkOpcodeStringTable[] = {
#define CRANK_TALK_OPCODE(x) #x,
#include "crank-talk/opcodes.inc"
#undef CRANK_TALK_OPCODE
};

CRANK_TALK_API const char *
crank_talk_Opcode_toString(crank_talk_Opcode_t opcode)
{
    return CrankTalkOpcodeStringTable[opcode];
}

CRANK_TALK_API const char *
crank_talk_OperandVectorName_toString(crank_talk_OperandVectorName_t vectorName)
{
    switch(vectorName)
    {
    case CrankTalkOperandVectorArguments: return "arguments";
    case CrankTalkOperandVectorCaptures:  return "captures";
    case CrankTalkOperandVectorLiteral:   return "literals";
    case CrankTalkOperandVectorTemporary: return "temps";
    default: abort();
    }
}

CRANK_TALK_API crank_talk_BytecodeAssembler_t *
crank_talk_BytecodeAssembler_create(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t assemblerValue = CrankAllocateValuesObjectWithClassName(context, BytecodeAssembler);
    crank_talk_BytecodeAssembler_t *assembler = (crank_talk_BytecodeAssembler_t*)assemblerValue;
    assembler->byteStream = crank_talk_ByteStream_create(context);
    assembler->literals = crank_talk_OrderedCollection_create(context);
    assembler->temporaryCount = crank_talk_oop_encodeSmallInteger(0);
    return assembler;
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_finishBuildingInFunction(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_CompiledFunction_t *compiledFunction)
{
    compiledFunction->literalVector = (crank_talk_Array_t*)crank_talk_OrderedCollection_asArray(context, assembler->literals);
    compiledFunction->temporaryCount = assembler->temporaryCount;
    compiledFunction->bytecode = (crank_talk_ByteArray_t*)crank_talk_ByteStream_asByteArray(context, assembler->byteStream);
}

CRANK_TALK_API crank_talk_BytecodeLabel_t *
crank_talk_BytecodeAssembler_newLabel(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t labelValue = CrankAllocateValuesObjectWithClassName(context, BytecodeLabel);
    crank_talk_BytecodeLabel_t *label = (crank_talk_BytecodeLabel_t *)labelValue;
    label->isDefined = crank_talk_oop_fromBoolean(context, false);
    label->destinationValue = crank_talk_oop_encodeSmallInteger(0);
    label->pendingLocations = crank_talk_OrderedCollection_create(context);
    return label;
}

CRANK_TALK_API crank_talk_BytecodeVectorOperand_t
crank_talk_BytecodeAssembler_getArgumentOperand(size_t index)
{
    return (index << CrankTalkOperandVectorBits) | CrankTalkOperandVectorArguments;
}

CRANK_TALK_API crank_talk_BytecodeVectorOperand_t
crank_talk_BytecodeAssembler_getCaptureOperand(size_t index)
{
    return (index << CrankTalkOperandVectorBits) | CrankTalkOperandVectorCaptures;
}

CRANK_TALK_API crank_talk_BytecodeVectorOperand_t
crank_talk_BytecodeAssembler_getOrCreateLiteral(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_oop_t literalValue)
{
    size_t literalCount = crank_talk_OrderedCollection_getSize(assembler->literals);
    for(size_t i = 1; i <= literalCount; ++i)
    {
        crank_talk_oop_t existingLiteral = crank_talk_OrderedCollection_at(context, assembler->literals, i);
        if(literalValue == existingLiteral)
            return (crank_talk_BytecodeVectorOperand_t)(((i - 1) << CrankTalkOperandVectorBits) | CrankTalkOperandVectorLiteral);
    }

    crank_talk_OrderedCollection_add(context, assembler->literals, literalValue);
    return (crank_talk_BytecodeVectorOperand_t)((literalCount << CrankTalkOperandVectorBits) | CrankTalkOperandVectorLiteral);
}

CRANK_TALK_API crank_talk_BytecodeVectorOperand_t
crank_talk_BytecodeAssembler_newTemporary(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler)
{
    size_t temporaryCount = crank_talk_oop_decodeSmallInteger(assembler->temporaryCount) + 1;
    assembler->temporaryCount = crank_talk_oop_encodeSmallInteger(temporaryCount);
    return (crank_talk_BytecodeVectorOperand_t)((temporaryCount << CrankTalkOperandVectorBits) | CrankTalkOperandVectorTemporary);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_opcode(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_Opcode_t opcode, crank_talk_oop_t sourcePosition)
{
    crank_talk_ByteStream_insertByte(context, assembler->byteStream, (uint8_t)opcode);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_vectorOperand(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeVectorOperand_t operand)
{
    crank_talk_ByteStream_insertByte(context, assembler->byteStream, operand & 0xFF);
    crank_talk_ByteStream_insertByte(context, assembler->byteStream, (operand >> 8) & 0xFF);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_labelOperand(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeLabel_t *label)
{
    size_t labelPosition = crank_talk_ByteStream_getSize(assembler->byteStream);
    size_t pc = labelPosition + 2;
    if(crank_talk_oop_asBoolean(context, label->isDefined))
    {
        size_t labelValue = crank_talk_oop_encodeSmallInteger(label->destinationValue);
        int16_t relativeOffset = (int16_t)(labelValue - pc);
        crank_talk_ByteStream_insertByte(context, assembler->byteStream, relativeOffset & 0xFF);
        crank_talk_ByteStream_insertByte(context, assembler->byteStream, (relativeOffset >> 8) & 0xFF);
    }
    else
    {
        crank_talk_OrderedCollection_add(context, label->pendingLocations, crank_talk_oop_encodeSmallInteger(labelPosition));
        crank_talk_ByteStream_insertByte(context, assembler->byteStream, 0);
        crank_talk_ByteStream_insertByte(context, assembler->byteStream, 0);
    }
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_putLabel(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeLabel_t *label)
{
    assert(!crank_talk_oop_asBoolean(context, label->isDefined));

    size_t byteStreamSize = crank_talk_ByteStream_getSize(assembler->byteStream);
    label->isDefined = crank_talk_oop_fromBoolean(context, true);
    size_t destinationValue = byteStreamSize;
    label->destinationValue = crank_talk_oop_encodeSmallInteger(destinationValue);
    size_t pendingLocationCount = crank_talk_OrderedCollection_getSize(label->pendingLocations);
    for(size_t i = 1; i <= pendingLocationCount; ++i)
    {
        size_t pendingLocation = crank_talk_oop_decodeSmallInteger(crank_talk_OrderedCollection_at(context, label->pendingLocations, i));
        size_t pendingLocationPC = pendingLocation + 2;
        assert(pendingLocationPC <= byteStreamSize);

        int16_t relativeOffset = (int16_t)(destinationValue - pendingLocationPC);
        crank_talk_ByteStream_atPut(assembler->byteStream, pendingLocation + 1, relativeOffset & 0xFF);
        crank_talk_ByteStream_atPut(assembler->byteStream, pendingLocation + 2, (relativeOffset >> 8) & 0xFF);
    }
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_nop(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeNop, sourcePosition);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_breakpoint(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeBreakpoint, sourcePosition);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_unreachable(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeUnreachable, sourcePosition);
}
CRANK_TALK_API void
crank_talk_BytecodeAssembler_beginSend(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeBeginSend, sourcePosition);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_alloca(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeVectorOperand_t resultTemporary, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeAlloca, sourcePosition);
    crank_talk_BytecodeAssembler_vectorOperand(context, assembler, resultTemporary);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_loadFromVariable(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeVectorOperand_t resultTemporary, crank_talk_BytecodeVectorOperand_t variable, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeLoadFromVariable, sourcePosition);
    crank_talk_BytecodeAssembler_vectorOperand(context, assembler, variable);
    crank_talk_BytecodeAssembler_vectorOperand(context, assembler, resultTemporary);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_jump(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeLabel_t *destinationLabel, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeJump, sourcePosition);
    crank_talk_BytecodeAssembler_labelOperand(context, assembler, destinationLabel);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_return(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeVectorOperand_t resultValue, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeReturn, sourcePosition);
    crank_talk_BytecodeAssembler_vectorOperand(context, assembler, resultValue);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_callArgument(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeVectorOperand_t argumentValue, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeCallArgument, sourcePosition);
    crank_talk_BytecodeAssembler_vectorOperand(context, assembler, argumentValue);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_sendMessage(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeVectorOperand_t resultTemporary, crank_talk_BytecodeVectorOperand_t selectorValue, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeSendMessage, sourcePosition);
    crank_talk_BytecodeAssembler_vectorOperand(context, assembler, selectorValue);
    crank_talk_BytecodeAssembler_vectorOperand(context, assembler, resultTemporary);
}

CRANK_TALK_API void
crank_talk_BytecodeAssembler_storeInVariable(crank_talk_VMContext_t *context, crank_talk_BytecodeAssembler_t *assembler, crank_talk_BytecodeVectorOperand_t value, crank_talk_BytecodeVectorOperand_t variable, crank_talk_oop_t sourcePosition)
{
    crank_talk_BytecodeAssembler_opcode(context, assembler, CrankTalkOpcodeStoreInVariable, sourcePosition);
    crank_talk_BytecodeAssembler_vectorOperand(context, assembler, value);
    crank_talk_BytecodeAssembler_vectorOperand(context, assembler, variable);
}

CRANK_TALK_INLINE int16_t
crank_talk_BytecodeFunction_fetchInt16(size_t bytecodesSize, size_t *pc, uint8_t *bytecodes)
{
    assert(*pc + 2 <= bytecodesSize);
    uint8_t firstByte  = bytecodes[*pc]; *pc += 1;
    uint8_t secondByte = bytecodes[*pc]; *pc += 1;
    return firstByte | (secondByte << 8);
}

CRANK_TALK_INLINE uint16_t
crank_talk_BytecodeFunction_fetchUInt16(size_t bytecodesSize, size_t *pc, uint8_t *bytecodes)
{
    assert(*pc + 2 <= bytecodesSize);
    uint8_t firstByte  = bytecodes[*pc]; *pc += 1;
    uint8_t secondByte = bytecodes[*pc]; *pc += 1;
    return firstByte | (secondByte << 8);
}

static void
crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(size_t bytecodesSize, size_t *pc, uint8_t *bytecodes)
{
    uint16_t vectorOperand = crank_talk_BytecodeFunction_fetchUInt16(bytecodesSize, pc, bytecodes);
    crank_talk_OperandVectorName_t vectorName = vectorOperand & CrankTalkOperandVectorBitMask;
    uint32_t vectorIndex = vectorOperand >> CrankTalkOperandVectorBits;

    printf("%s@%d", crank_talk_OperandVectorName_toString(vectorName), vectorIndex);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_vm_BytecodeFunction_apply(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction, crank_talk_BlockClosure_t *closureValue, size_t argumentCount, crank_talk_oop_t *arguments)
{
    crank_talk_stackFrameBytecodeFunctionActivationRecord_t activationRecord = {
        .super.type = CrankTalkStackFrameRecordBytecodeFunctionActivationRecord
    };
    activationRecord.function = (crank_talk_oop_t)compiledFunction;
    activationRecord.bytecode = (crank_talk_oop_t)compiledFunction->bytecode;

    activationRecord.literalVector = (crank_talk_oop_t)compiledFunction->literalVector;
    if(closureValue)
        activationRecord.captureVector = (crank_talk_oop_t)closureValue->captureVector;

    activationRecord.argumentCount = argumentCount;
    activationRecord.arguments = arguments;

    activationRecord.temporaryCount = crank_talk_oop_decodeSmallInteger(compiledFunction->temporaryCount);
    activationRecord.temporaries = alloca(activationRecord.temporaryCount*sizeof(crank_talk_oop_t));
    memset(activationRecord.temporaries, 0, activationRecord.temporaryCount*sizeof(crank_talk_oop_t));

    crank_talk_stackFrame_pushRecord(&activationRecord.super);

    if(!_setjmp(activationRecord.jmpbuffer))
        crank_talk_vm_BytecodeFunction_interpretWithActivationRecord(context, &activationRecord);

    crank_talk_stackFrame_popRecord(&activationRecord.super);
    return activationRecord.result;
}


CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_BytecodeFunction_fetchVectorOperand(crank_talk_stackFrameBytecodeFunctionActivationRecord_t *activationRecord)
{
    size_t bytecodesSize = crank_talk_object_getSizeInBytes(activationRecord->bytecode);
    uint8_t *bytecodes = crank_talk_oop_getBytesData(activationRecord->bytecode);
    uint16_t vectorOperand = crank_talk_BytecodeFunction_fetchUInt16(bytecodesSize, &activationRecord->pc, bytecodes);
    crank_talk_OperandVectorName_t vectorName = vectorOperand & CrankTalkOperandVectorBitMask;
    uint32_t vectorIndex = vectorOperand >> CrankTalkOperandVectorBits;

    switch (vectorName)
    {
    case CrankTalkOperandVectorArguments:
        assert(vectorIndex < activationRecord->argumentCount);
        return activationRecord->arguments[vectorIndex];
    case CrankTalkOperandVectorCaptures:
        assert(vectorIndex < crank_talk_object_getSizeInSlots(activationRecord->captureVector));
        return crank_talk_oop_getSlotsData(activationRecord->captureVector)[vectorIndex];
    case CrankTalkOperandVectorLiteral:
        assert(vectorIndex < crank_talk_object_getSizeInSlots(activationRecord->literalVector));
        return crank_talk_oop_getSlotsData(activationRecord->literalVector)[vectorIndex];
    case CrankTalkOperandVectorTemporary:
        assert(vectorIndex <= activationRecord->temporaryCount);
        if(vectorIndex == 0)
            return crank_talk_oop_nil();
        return activationRecord->temporaries[vectorIndex - 1];

    default: abort();
    }

    abort();
}

CRANK_TALK_INLINE void
crank_talk_BytecodeFunction_fetchTemporaryAndSetValue(crank_talk_stackFrameBytecodeFunctionActivationRecord_t *activationRecord, crank_talk_oop_t value)
{
    size_t bytecodesSize = crank_talk_object_getSizeInBytes(activationRecord->bytecode);
    uint8_t *bytecodes = crank_talk_oop_getBytesData(activationRecord->bytecode);
    uint16_t vectorOperand = crank_talk_BytecodeFunction_fetchUInt16(bytecodesSize, &activationRecord->pc, bytecodes);
    crank_talk_OperandVectorName_t vectorName = vectorOperand & CrankTalkOperandVectorBitMask;
    uint32_t vectorIndex = vectorOperand >> CrankTalkOperandVectorBits;
    if((vectorName != CrankTalkOperandVectorTemporary) || (vectorIndex > activationRecord->temporaryCount))
        crank_talk_error("Expected a valid temporary vector.");
    if(vectorIndex != 0)
        activationRecord->temporaries[vectorIndex - 1] = value;
}

CRANK_TALK_API void
crank_talk_vm_BytecodeFunction_interpretWithActivationRecord(crank_talk_VMContext_t *context, crank_talk_stackFrameBytecodeFunctionActivationRecord_t *activationRecord)
{
    size_t bytecodesSize = crank_talk_object_getSizeInBytes(activationRecord->bytecode);
    uint8_t *bytecodes = crank_talk_oop_getBytesData(activationRecord->bytecode);

    activationRecord->pc = 0;
    crank_talk_gc_safepoint(context);

    while(activationRecord->pc < bytecodesSize)
    {
        crank_talk_Opcode_t opcode = bytecodes[activationRecord->pc++];

        switch (opcode)
        {
        // Zero operands
        case CrankTalkOpcodeNop:
            break;

        case CrankTalkOpcodeBreakpoint:
            crank_talk_error("TODO: breakpoint");
            break;
        case CrankTalkOpcodeUnreachable:
            crank_talk_error("Unreachable reached");
            break;

        case CrankTalkOpcodeAlloca:
        {
            crank_talk_oop_t localVariableValue = CrankAllocateValuesObjectWithClassName(context, LocalVariable);
            crank_talk_BytecodeFunction_fetchTemporaryAndSetValue(activationRecord, localVariableValue);
        } break;

        case CrankTalkOpcodeBeginSend:
            activationRecord->calloutArgumentCount = 0;
            break;

        // One operands
        case CrankTalkOpcodeLoadFromVariable:
        {
            crank_talk_oop_t variable = crank_talk_BytecodeFunction_fetchVectorOperand(activationRecord);
            crank_talk_LiteralVariable_t *literalVariable = (crank_talk_LiteralVariable_t *)variable;
            crank_talk_BytecodeFunction_fetchTemporaryAndSetValue(activationRecord, literalVariable->value);
        } break;
        case CrankTalkOpcodeJump:
        {
            int16_t offset = crank_talk_BytecodeFunction_fetchInt16(bytecodesSize, &activationRecord->pc, bytecodes);
            activationRecord->pc += offset;
            if(offset < 0)
                crank_talk_gc_safepoint(context);
        } break;
        case CrankTalkOpcodeReturn:
        {
            activationRecord->result = crank_talk_BytecodeFunction_fetchVectorOperand(activationRecord);
            crank_talk_gc_safepoint(context);
            return;
        } break;

        case CrankTalkOpcodeCallArgument:
        {
            assert(activationRecord->calloutArgumentCount < CRANK_TALK_MAX_CALL_ARGUMENT_COUNT);
            activationRecord->calloutArguments[activationRecord->calloutArgumentCount++] = crank_talk_BytecodeFunction_fetchVectorOperand(activationRecord);
        } break;
        case CrankTalkOpcodeSendMessage:
        {
            crank_talk_oop_t selector = crank_talk_BytecodeFunction_fetchVectorOperand(activationRecord);
            crank_talk_oop_t result = crank_talk_send(context, selector, activationRecord->calloutArgumentCount, activationRecord->calloutArguments);
            activationRecord->calloutArgumentCount = 0;
            crank_talk_BytecodeFunction_fetchTemporaryAndSetValue(activationRecord, result);
        } break;

        // Two operands
        case CrankTalkOpcodeStoreInVariable:
        {
            crank_talk_oop_t valueToStore = crank_talk_BytecodeFunction_fetchVectorOperand(activationRecord);
            crank_talk_oop_t variable = crank_talk_BytecodeFunction_fetchVectorOperand(activationRecord);
            crank_talk_LiteralVariable_t *literalVariable = (crank_talk_LiteralVariable_t *)variable;
            literalVariable->value = valueToStore;
        } break;

        
        // Unsupported opcode
        default:
            crank_talk_errorPrintf(context, "Unsupported opcode %d[%s].", opcode, crank_talk_Opcode_toString(opcode));
            break;
        }
    }
    abort();
}

CRANK_TALK_API void
crank_talk_BytecodeFunction_dumpToConsole(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction)
{
    size_t argumentCount = crank_talk_oop_decodeSmallInteger(compiledFunction->argumentCount);
    size_t literalCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)compiledFunction->literalVector);
    size_t temporaryCount = crank_talk_oop_decodeSmallInteger(compiledFunction->temporaryCount);

    crank_talk_oop_t *literals = compiledFunction->literalVector->elements;
    printf("function argumentCount %d temporaryCount %d literals {\n", (int)argumentCount, (int)temporaryCount);
    for(size_t i = 0; i < literalCount; ++i)
    {
        crank_talk_oop_t literalString = crank_talk_oop_printString(context, literals[i]);
        printf("  %d: "CRANK_TALK_STRING_FORMAT"\n", (int)i, CRANK_TALK_STRING_FORMAT_ARG(literalString));
    }
    size_t bytecodesSize = crank_talk_object_getSizeInBytes((crank_talk_oop_t)compiledFunction->bytecode);
    uint8_t *bytecodes = compiledFunction->bytecode->elements;
    size_t pc = 0;
    printf("} bytecodes {\n");
    while(pc < bytecodesSize)
    {
        printf("  %04x: ", (int)pc);
        crank_talk_Opcode_t opcode = bytecodes[pc++];
        printf("%s", crank_talk_Opcode_toString(opcode));

        switch (opcode)
        {
        // Zero operands
        case CrankTalkOpcodeNop:
        case CrankTalkOpcodeBreakpoint:
        case CrankTalkOpcodeUnreachable:
        case CrankTalkOpcodeBeginSend:
            break;

        case CrankTalkOpcodeAlloca:
            printf(" -> ");
            crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(bytecodesSize, &pc, bytecodes);
            break;

        // One operand
        case CrankTalkOpcodeLoadFromVariable:
        {
            printf(" *");
            crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(bytecodesSize, &pc, bytecodes);
            printf(" -> ");
            crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(bytecodesSize, &pc, bytecodes);
        }   break;
        case CrankTalkOpcodeJump:
        {
            int16_t offset = crank_talk_BytecodeFunction_fetchInt16(bytecodesSize, &pc, bytecodes);
            uint32_t destinationPC = pc + offset;
            printf(" %04x -> %04x", offset, destinationPC);
        }   break;
        case CrankTalkOpcodeReturn:
            printf(" ");
            crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(bytecodesSize, &pc, bytecodes);
            break;
        case CrankTalkOpcodeCallArgument:
            printf(" ");
            crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(bytecodesSize, &pc, bytecodes);
            break;
        
        case CrankTalkOpcodeSendMessage:
            printf(" ");
            crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(bytecodesSize, &pc, bytecodes);
            printf(" -> ");
            crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(bytecodesSize, &pc, bytecodes);
            break;
        // Two operands
        case CrankTalkOpcodeStoreInVariable:
            printf(" ");
            crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(bytecodesSize, &pc, bytecodes);
            printf(" -> *");
            crank_talk_BytecodeFunction_fetchAndPrintVectorOperand(bytecodesSize, &pc, bytecodes);
            break;

        // Unsupported bytecode
        default:
            printf(" Unsupported opcode %d", opcode);
            break;
        }

        printf("\n");
    }

    printf("}\n");


}