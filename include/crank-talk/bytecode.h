#ifndef CRANK_TALK_BYTECODE_H
#define CRANK_TALK_BYTECODE_H
#pragma once

#include "object-model.h"
#include "stack-frame.h"

typedef enum crank_talk_Opcode_e crank_talk_Opcode_t;
typedef enum crank_talk_OperandVectorName_e crank_talk_OperandVectorName_t;

enum crank_talk_Opcode_e
{
#define CRANK_TALK_OPCODE(x) SysmelVMOpcode ## x,
#include "opcodes.inc"
#undef CRANK_TALK_OPCODE
};

typedef enum crank_talk_OperandVectorName_e
{
    SysmelVMOperandVectorTemporary = 0,
    SysmelVMOperandVectorArguments,
    SysmelVMOperandVectorCaptures,
    SysmelVMOperandVectorLiteral,
    SysmelVMOperandVectorBits = 2,
    SysmelVMOperandVectorBitMask = 3,
} crank_talk_OperandVectorName_t;

CRANK_TALK_API const char *crank_talk_OperandVectorName_toString(crank_talk_OperandVectorName_t vectorName);

typedef struct crank_talk_BytecodeAssembler_s crank_talk_BytecodeAssembler_t;
typedef struct crank_talk_BytecodeLabel_s crank_talk_BytecodeLabel_t;

struct crank_talk_BytecodeAssembler_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t byteStream;
    crank_talk_oop_t literals;
    crank_talk_oop_t temporaryCount;
    crank_talk_oop_t currentSourcePosition;
};

CRANK_TALK_API const char *crank_talk_Opcode_toString(crank_talk_Opcode_t opcode);

#endif //CRANK_TALK_BYTECODE_H
