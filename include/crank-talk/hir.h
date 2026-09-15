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
        typedef struct crank_talk_HIRAllocaInstruction_s crank_talk_HIRAllocaInstruction_t;
        typedef struct crank_talk_HIRBranchInstruction_s crank_talk_HIRBranchInstruction_t;
        typedef struct crank_talk_HIRConditionalBranchInstruction_s crank_talk_HIRConditionalBranchInstruction_t;
        typedef struct crank_talk_HIRMakeClosureInstruction_s crank_talk_HIRMakeClosureInstruction_t;
        typedef struct crank_talk_HIRPhiInstruction_s crank_talk_HIRPhiInstruction_t;
        typedef struct crank_talk_HIRPhiSourceInstruction_s crank_talk_HIRPhiSourceInstruction_t;
        typedef struct crank_talk_HIRReturnInstruction_s crank_talk_HIRReturnInstruction_t;
        typedef struct crank_talk_HIRSendInstruction_s crank_talk_HIRSendInstruction_t;
        typedef struct crank_talk_HIRSlotLoadInstruction_s crank_talk_HIRSlotLoadInstruction_t;
        typedef struct crank_talk_HIRSlotStoreInstruction_s crank_talk_HIRSlotStoreInstruction_t;
        typedef struct crank_talk_HIRUnreachableInstruction_s crank_talk_HIRUnreachableInstruction_t;
        typedef struct crank_talk_HIRVariableLoadInstruction_s crank_talk_HIRVariableLoadInstruction_t;
        typedef struct crank_talk_HIRVariableStoreInstruction_s crank_talk_HIRVariableStoreInstruction_t;

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
    crank_talk_Symbol_t *name;
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
    crank_talk_HIRFunctionLocal_t *capturedValue;
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

struct crank_talk_HIRAllocaInstruction_s
{
    crank_talk_HIRInstruction_t super;
};

struct crank_talk_HIRBranchInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRBasicBlock_t *destination;
};

struct crank_talk_HIRConditionalBranchInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRValue_t *condition;
    crank_talk_HIRBasicBlock_t *trueDestination;
    crank_talk_HIRBasicBlock_t *falseDestination;
};

struct crank_talk_HIRMakeClosureInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_CompiledFunction_t *function;
    crank_talk_Array_t *capturedValues;
};

struct crank_talk_HIRPhiInstruction_s
{
    crank_talk_HIRInstruction_t super;
};

struct crank_talk_HIRPhiSourceInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRPhiInstruction_t *targetPhi;
    crank_talk_HIRValue_t *value;
};

struct crank_talk_HIRReturnInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRValue_t *resultValue;
};

struct crank_talk_HIRSendInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRValue_t *selector;
    crank_talk_Array_t *receiverWithArguments;
};

struct crank_talk_HIRSlotLoadInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRValue_t *receiver;
    crank_talk_oop_t slotIndex;
};

struct crank_talk_HIRSlotStoreInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRValue_t *receiver;
    crank_talk_oop_t slotIndex;
    crank_talk_HIRValue_t *value;
};

struct crank_talk_HIRUnreachableInstruction_s
{
    crank_talk_HIRInstruction_t super;
};

struct crank_talk_HIRVariableLoadInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRValue_t *variable;
};

struct crank_talk_HIRVariableStoreInstruction_s
{
    crank_talk_HIRInstruction_t super;
    crank_talk_HIRValue_t *variable;
    crank_talk_HIRValue_t *value;
};

struct crank_talk_HIRBuilder_s
{
    crank_talk_Object_t super;
    crank_talk_HIRFunction_t *hirFunction;
    crank_talk_HIRBasicBlock_t *basicBlock;
    crank_talk_HIRBuilder_t *allocaBuilder;
    crank_talk_oop_t lexicalEnvironment;
};

CRANK_TALK_API crank_talk_oop_t crank_talk_HIRConstantLiteralValue_create(crank_talk_VMContext_t *context, crank_talk_oop_t value, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRConstantControlFlowEscapeValue_create(crank_talk_VMContext_t *context, crank_talk_oop_t sourcePosition);

CRANK_TALK_API void crank_talk_HIRFunction_ensureEnumeratedLocalValues(crank_talk_VMContext_t *context, crank_talk_HIRFunction_t *functionValue);
CRANK_TALK_API void crank_talk_HIRFunction_dumpToStream(crank_talk_VMContext_t *context, crank_talk_HIRFunction_t *functionValue, crank_talk_oop_t stringStream);
CRANK_TALK_API void crank_talk_HIRFunction_dumpToConsole(crank_talk_VMContext_t *context, crank_talk_HIRFunction_t *functionValue);

CRANK_TALK_API void crank_talk_HIRFunction_addBasicBlock(crank_talk_HIRFunction_t *function, crank_talk_HIRBasicBlock_t *basicBlock);
CRANK_TALK_API void crank_talk_HIRBasicBlock_addInstruction(crank_talk_HIRBasicBlock_t *basicBlock, crank_talk_HIRInstruction_t *instruction);

CRANK_TALK_API crank_talk_HIRBasicBlock_t *crank_talk_HIRBasicBlock_create(crank_talk_VMContext_t *context);
CRANK_TALK_API crank_talk_HIRBasicBlock_t *crank_talk_HIRBasicBlock_createNamed(crank_talk_VMContext_t *context, const char *name);

CRANK_TALK_API bool crank_talk_HIRInstruction_hasResultValue(crank_talk_VMContext_t *context, crank_talk_HIRInstruction_t *instruction);

CRANK_TALK_API crank_talk_HIRBuilder_t *crank_talk_HIRBuilder_create(crank_talk_VMContext_t *context, crank_talk_HIRFunction_t *function, crank_talk_HIRBasicBlock_t *basicBlock, crank_talk_HIRBuilder_t *allocaBuilder, crank_talk_oop_t lexicalEnvironment);
CRANK_TALK_API crank_talk_HIRBuilder_t *crank_talk_HIRBuilder_copyWithBasicBlock(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRBasicBlock_t *basicBlock);
CRANK_TALK_API void crank_talk_HIRBuilder_addInstruction(crank_talk_HIRBuilder_t *builder, crank_talk_HIRInstruction_t *instruction);
CRANK_TALK_API bool crank_talk_HIRBuilder_isLastTerminator(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder);

CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_alloca(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_branch(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRBasicBlock_t *destination, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_conditionalBranch(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *condition, crank_talk_HIRBasicBlock_t *trueDestination, crank_talk_HIRBasicBlock_t *falseDestination, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_makeClosure(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_CompiledFunction_t *function, crank_talk_Array_t *capturedValues, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_HIRPhiInstruction_t *crank_talk_HIRBuilder_phi(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_phiSource(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRPhiInstruction_t *targetPhi, crank_talk_HIRValue_t *value, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_return(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *resultValue, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_send(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *selector, crank_talk_Array_t *receiverWithArguments, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_slotLoad(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *receiver, size_t slotIndex, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_slotStore(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *receiver, size_t slotIndex, crank_talk_HIRValue_t *value, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_unreachable(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_variableLoad(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *variable, crank_talk_oop_t sourcePosition);
CRANK_TALK_API crank_talk_oop_t crank_talk_HIRBuilder_variableStore(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder, crank_talk_HIRValue_t *variable, crank_talk_HIRValue_t *value, crank_talk_oop_t sourcePosition);

CRANK_TALK_API uint16_t crank_talk_HIRValue_getBytecodeAssemblyOperandWith(crank_talk_VMContext_t *context, crank_talk_HIRValue_t *value, crank_talk_oop_t bytecodeAssembler);
CRANK_TALK_API void crank_talk_HIRInstruction_emitBytecodeAssemblyWith(crank_talk_VMContext_t *context, crank_talk_HIRInstruction_t *instruction, crank_talk_oop_t bytecodeAssembler);

//struct crank_talk_HIRSlotLoadInstruction_s
//{
//    crank_talk_HIRInstruction_t super;
//    crank_talk_HIRValue_t *receiver;
//    crank_talk_oop_t slotIndex;
//};

#endif //CRANK_TALK_HIR_H