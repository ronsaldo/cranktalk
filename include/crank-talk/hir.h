#ifndef CRANK_TALK_HIR_H
#define CRANK_TALK_HIR_H

#include "object-model.h"

typedef struct crank_talk_HIRValue_s crank_talk_HIRValue_t;
typedef struct crank_talk_HIRConstantValue_s crank_talk_HIRConstantValue_t;
typedef struct crank_talk_HIRConstantLiteralValue_s crank_talk_HIRConstantLiteralValue_t;
typedef struct crank_talk_HIRConstantControlFlowEscapeValue_s crank_talk_HIRConstantControlFlowEscapeValue_t;

typedef struct crank_talk_HIRFunction_s crank_talk_HIRFunction_t;
typedef struct crank_talk_HIRFunctionLocal_s crank_talk_HIRFunctionLocal_t;
    typedef struct crank_talk_HIRArgument_s crank_talk_HIRArgument_t;
    typedef struct crank_talk_HIRCapture_s crank_talk_HIRCapture_t;
    typedef struct crank_talk_HIRBasicBlock_s crank_talk_HIRBasicBlock_t;
    typedef struct crank_talk_HIRInstruction_s crank_talk_HIRInstruction_t;
        typedef struct crank_talk_HIRBranchInstruction_s crank_talk_HIRBranchInstruction_t;
        typedef struct crank_talk_HIRReturnInstruction_s crank_talk_HIRReturnInstruction_t;
        typedef struct crank_talk_HIRUnreachableInstruction_s crank_talk_HIRUnreachableInstruction_t;

typedef struct crank_talk_HIRBuilder_s crank_talk_HIRBuilder_t;
    
struct crank_talk_HIRValue_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t sourcePosition;
};

struct crank_talk_HIRConstantValue_s
{
    crank_talk_HIRValue_t super;
};

struct crank_talk_HIRConstantLiteralValue_s
{
    crank_talk_HIRConstantValue_t super;
    crank_talk_oop_t value;
};

struct crank_talk_HIRConstantControlFlowEscapeValue_s
{
    crank_talk_HIRConstantValue_t super;
};

struct crank_talk_HIRFunction_s
{
    crank_talk_HIRConstantValue_t super;
    crank_talk_oop_t name;
    crank_talk_Array_t *arguments;
    crank_talk_Array_t *captures;
    crank_talk_HIRBasicBlock_t *firstBasicBlock;
    crank_talk_HIRBasicBlock_t *lastBasicBlock;
    crank_talk_Array_t *enumeratedLocalValues;
};

struct crank_talk_HIRFunctionLocal_s
{
    crank_talk_HIRValue_t super;
    crank_talk_oop_t name;
    crank_talk_oop_t index;

    crank_talk_oop_t bytecodeVectorIndex;
};

struct crank_talk_HIRArgument_s
{
    crank_talk_HIRFunctionLocal_t super;
};

struct crank_talk_HIRCapture_s
{
    crank_talk_HIRFunctionLocal_t super;
};

struct crank_talk_HIRBasicBlock_s
{
    crank_talk_HIRFunctionLocal_t super;
    crank_talk_HIRBasicBlock_t *nextBlock;
    crank_talk_HIRBasicBlock_t *previousBlock;

    crank_talk_HIRInstruction_t *firstInstruction;
    crank_talk_HIRInstruction_t *lastInstruction;

    crank_talk_oop_t bytecodeLabel;
};

struct crank_talk_HIRInstruction_s
{
    crank_talk_HIRFunctionLocal_t super;
    crank_talk_HIRInstruction_t *nextInstruction;
    crank_talk_HIRInstruction_t *previousInstruction;
};

struct crank_talk_HIRBranchInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRBasicBlock_t *destination;
};

struct crank_talk_HIRReturnInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRValue_t *resultValue;
};

struct crank_talk_HIRUnreachableInstruction_s
{
    crank_talk_HIRInstruction_t super;
};

struct crank_talk_HIRBuilder_s
{
    crank_talk_Object_t super;
    crank_talk_HIRFunction_t *hirFunction;
    crank_talk_HIRBasicBlock_t *basicBlock;
    crank_talk_HIRBuilder_t *allocaBuilder;
    crank_talk_oop_t lexicalEnvironment;
};

#endif //CRANK_TALK_HIR_H