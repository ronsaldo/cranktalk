#include "crank-talk/bytecode.h"
#include <stdlib.h>

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
    case SysmelVMOperandVectorArguments: return "arguments";
    case SysmelVMOperandVectorCaptures:  return "captures";
    case SysmelVMOperandVectorLiteral:   return "literals";
    case SysmelVMOperandVectorTemporary: return "temps";
    default: abort();
    }
}