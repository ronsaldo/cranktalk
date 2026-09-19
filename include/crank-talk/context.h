#ifndef CRANK_TALK_CONTEXT_H
#define CRANK_TALK_CONTEXT_H

#include "object-model.h"
#include "heap.h"
#include "dynarray.h"

#define CrankAllocateVariadicObjectWithClassName(context, objectClassName, dataType, variadicByteSize) \
    crank_talk_heap_allocateOopWithSizeAndType(&context->heap, sizeof(crank_talk_ ## objectClassName##_t) + variadicByteSize, dataType, context->roots.objectClassName##Class)

#define CrankAllocateBytesObjectWithClassName(context, objectClassName) CrankAllocateVariadicObjectWithClassName(context, objectClassName, CrankTalkGCObjectDataTypeBytes, 0)
#define CrankAllocateValuesObjectWithClassName(context, objectClassName) CrankAllocateVariadicObjectWithClassName(context, objectClassName, CrankTalkGCObjectDataTypeOops, 0)

typedef struct crank_talk_VMContext_s crank_talk_VMContext_t;
typedef struct crank_talk_VMContextRoots_s crank_talk_VMContextRoots_t;

typedef struct crank_talk_VMContext_s crank_talk_VMContext_t;
typedef struct crank_talk_VMContextRoots_s crank_talk_VMContextRoots_t;

struct crank_talk_VMContextRoots_s
{
    crank_talk_oop_t ProtoObjectClass;
    crank_talk_oop_t ObjectClass;
    crank_talk_oop_t BehaviorClass;
    crank_talk_oop_t ClassDescriptionClass;
    crank_talk_oop_t ClassClass;
    crank_talk_oop_t MetaclassClass;

    crank_talk_oop_t CompiledFunctionClass;
    crank_talk_oop_t BlockClosureClass;
    crank_talk_oop_t MessageClass;

    crank_talk_oop_t MagnitudeClass;
    crank_talk_oop_t CharacterClass;
    crank_talk_oop_t NumberClass;
    crank_talk_oop_t FloatClass;
    crank_talk_oop_t BoxedFloatClass;
    crank_talk_oop_t SmallFloatClass;
    crank_talk_oop_t FractionClass;
    crank_talk_oop_t IntegerClass;
    crank_talk_oop_t LargeIntegerClass;
    crank_talk_oop_t LargeNegativeIntegerClass;
    crank_talk_oop_t LargePositiveIntegerClass;
    crank_talk_oop_t SmallIntegerClass;

    crank_talk_oop_t UndefinedObjectClass;
    crank_talk_oop_t BooleanClass;
    crank_talk_oop_t FalseClass;
    crank_talk_oop_t TrueClass;

    crank_talk_oop_t TombstoneClass;

    crank_talk_oop_t VariableClass;
    crank_talk_oop_t LiteralVariableClass;
    crank_talk_oop_t GlobalVariableClass;
    crank_talk_oop_t LocalVariableClass;
    crank_talk_oop_t HIRVariableClass;
    crank_talk_oop_t SlotClass;

    crank_talk_oop_t LookupKeyClass;
    crank_talk_oop_t AssociationClass;

    crank_talk_oop_t SourceCodeClass;
    crank_talk_oop_t AbstractSourcePositionClass;
    crank_talk_oop_t EmptySourcePositionClass;
    crank_talk_oop_t SourcePositionClass;

    crank_talk_oop_t CrankTalkTokenClass;

    crank_talk_oop_t CollectionClass;
    crank_talk_oop_t HashedCollectionClass;
    crank_talk_oop_t DictionaryClass;
    crank_talk_oop_t IdentityDictionaryClass;
    crank_talk_oop_t SystemEnvironmentClass;
    crank_talk_oop_t MethodDictionaryClass;

    crank_talk_oop_t SetClass;
    crank_talk_oop_t IdentitySetClass;

    crank_talk_oop_t SequenceableCollectionClass;
    crank_talk_oop_t ArrayedCollectionClass;
    crank_talk_oop_t ArrayClass;
    crank_talk_oop_t ByteArrayClass;
    crank_talk_oop_t StringClass;
    crank_talk_oop_t SymbolClass;
    
    crank_talk_oop_t OrderedCollectionClass;

    crank_talk_oop_t ByteStreamClass;
    crank_talk_oop_t StringStreamClass;

    crank_talk_oop_t ExternalAddressClass;

    crank_talk_oop_t ShiftClassBuilderClass;

    crank_talk_oop_t SmalltalkImageClass;
    
    crank_talk_oop_t ContextClass;
    crank_talk_oop_t StackContextClass;
    crank_talk_oop_t MethodContextClass;
    crank_talk_oop_t ScriptContextClass;

    // System environment
    crank_talk_oop_t systemEnvironment;
    crank_talk_oop_t smalltalk;

    // Boolean values
    crank_talk_oop_t falseValue;
    crank_talk_oop_t trueValue;

    // Interned symbols
    crank_talk_oop_t internedSymbolSet;

    crank_talk_oop_t doesNotUnderstandSymbol;
    crank_talk_oop_t asStringSymbol;
    crank_talk_oop_t printStringSymbol;
    crank_talk_oop_t printOnSymbol;
    crank_talk_oop_t fullPrintOnSymbol;
    crank_talk_oop_t traverseAndCollectErrorsIntoSymbol;
    crank_talk_oop_t analyzeAndEvaluateWithSymbol;
    crank_talk_oop_t analyzeAndBuildWithSymbol;
    crank_talk_oop_t analyzeAndEvaluateIdentifierReferenceWithSymbol;
    crank_talk_oop_t analyzeAndBuildIdentifierReferenceWithSymbol;
    crank_talk_oop_t analyzeAndBuildAssignmentWithValueWithSymbol;
    crank_talk_oop_t lookupSymbolRecursively;
    crank_talk_oop_t lookupReceiverClassRecursively;
    crank_talk_oop_t lookupIsInnerBlockRecursively;

    crank_talk_oop_t hasResultValueSymbol;
    crank_talk_oop_t isTerminatorSymbol;
    crank_talk_oop_t getBytecodeAssemblyOperandWithSymbol;
    crank_talk_oop_t emitBytecodeAssemblyWithSymbol;
    crank_talk_oop_t isHIRVariableSymbol;

    crank_talk_oop_t valueSymbol;
    crank_talk_oop_t valueWithSymbol;

    crank_talk_oop_t classSymbol;
    crank_talk_oop_t notSymbol;
    crank_talk_oop_t equalsSymbol;
    crank_talk_oop_t identityEqualsSymbol;
    crank_talk_oop_t identityNotEqualsSymbol;
    crank_talk_oop_t selfSymbol;

    crank_talk_oop_t basicNewSymbol;
    crank_talk_oop_t basicNewWithSymbol;
    crank_talk_oop_t environmentSymbol;
    crank_talk_oop_t initializeSymbol;
    
    crank_talk_oop_t asSlotSymbol;

    crank_talk_oop_t plusSymbol;
    crank_talk_oop_t lessOrEqualsSymbol;

    crank_talk_oop_t processEventSymbol;

    // Message send macro dictionary.
    crank_talk_oop_t messageSendMacroDictionary;

    // Front end classes
    crank_talk_oop_t ParseTreeNodeClass;
    
    crank_talk_oop_t ParseTreeParseErrorNodeClass;
    crank_talk_oop_t ParseTreeAssignmentNodeClass;
    crank_talk_oop_t ParseTreeIdentifierReferenceNodeClass;
    crank_talk_oop_t ParseTreePseudoVariableReferenceNodeClass;
    
    crank_talk_oop_t ParseTreeNilReferenceNodeClass;
    crank_talk_oop_t ParseTreeFalseReferenceNodeClass;
    crank_talk_oop_t ParseTreeTrueReferenceNodeClass;
    crank_talk_oop_t ParseTreeSelfReferenceNodeClass;
    crank_talk_oop_t ParseTreeSuperReferenceNodeClass;
    crank_talk_oop_t ParseTreeThisContextReferenceNodeClass;

    crank_talk_oop_t ParseTreeArgumentDefinitionNodeClass;
    crank_talk_oop_t ParseTreeBlockNodeClass;
    crank_talk_oop_t ParseTreeLiteralNodeClass;
    
    crank_talk_oop_t ParseTreeCascadedMessageNodeClass;
    crank_talk_oop_t ParseTreeMessageCascadeNodeClass;
    crank_talk_oop_t ParseTreeMessageSendNodeClass;
    crank_talk_oop_t ParseTreeSequenceNodeClass;
    crank_talk_oop_t ParseTreeArrayNodeClass;
    crank_talk_oop_t ParseTreeReturnNodeClass;
    crank_talk_oop_t ParseTreeAddMethodNodeClass;
    crank_talk_oop_t ParseTreeMethodDefinitionNodeClass;
    crank_talk_oop_t ParseTreePragmaNodeClass;
    crank_talk_oop_t ParseTreeVariableDefinitionNodeClass;

    crank_talk_oop_t ParseTreeCullBlockNodeClass;
    crank_talk_oop_t ParseTreeIfSelectionNodeClass;
    crank_talk_oop_t ParseTreeWhileNodeClass;
    crank_talk_oop_t ParseTreeDoWhileNodeClass;

    crank_talk_oop_t ParseTreeToByDoNodeClass;

    // Environment
    crank_talk_oop_t AbstractEnvironmentClass;
    crank_talk_oop_t EmptyEnvironmentClass;
    crank_talk_oop_t SystemEnvironmentEnvironmentClass;
    crank_talk_oop_t LexicalEnvironmentClass;
    crank_talk_oop_t FunctionAnalysisEnvironmentClass;

    // HIR
    crank_talk_oop_t HIRValueClass;
    crank_talk_oop_t HIRConstantValueClass;
    crank_talk_oop_t HIRConstantLiteralValueClass;
    crank_talk_oop_t HIRConstantControlFlowEscapeValueClass;

    crank_talk_oop_t HIRFunctionClass;
    crank_talk_oop_t HIRFunctionLocalClass;
    crank_talk_oop_t HIRArgumentClass;
    crank_talk_oop_t HIRCaptureClass;
    crank_talk_oop_t HIRBasicBlockClass;
    crank_talk_oop_t HIRInstructionClass;
    crank_talk_oop_t HIRAllocaInstructionClass;
    crank_talk_oop_t HIRBranchInstructionClass;
    crank_talk_oop_t HIRConditionalBranchInstructionClass;
    crank_talk_oop_t HIRMakeClosureInstructionClass;
    crank_talk_oop_t HIRPhiInstructionClass;
    crank_talk_oop_t HIRPhiSourceInstructionClass;
    crank_talk_oop_t HIRNonLocalReturnInstructionClass;
    crank_talk_oop_t HIRReturnInstructionClass;
    crank_talk_oop_t HIRSendInstructionClass;
    crank_talk_oop_t HIRSuperSendInstructionClass;
    crank_talk_oop_t HIRSlotLoadInstructionClass;
    crank_talk_oop_t HIRSlotStoreInstructionClass;
    crank_talk_oop_t HIRThisContextInstructionClass;
    crank_talk_oop_t HIRUnreachableInstructionClass;
    crank_talk_oop_t HIRVariableLoadInstructionClass;
    crank_talk_oop_t HIRVariableStoreInstructionClass;

    crank_talk_oop_t HIRBuilderClass;

    // Passes
    crank_talk_oop_t AnalysisAndEvaluationContextClass;
    crank_talk_oop_t AnalysisAndBuildContextClass;

    // Bytecode assembler
    crank_talk_oop_t BytecodeAssemblerClass;
    crank_talk_oop_t BytecodeLabelClass;

    // IO
    crank_talk_oop_t StdioClass;
    crank_talk_oop_t BinaryFileStreamClass;
    
    crank_talk_oop_t stdinStream;
    crank_talk_oop_t stdoutStream;
    crank_talk_oop_t stderrStream;

    // Exceptions
    crank_talk_oop_t ExceptionClass;
    crank_talk_oop_t ErrorClass;
    crank_talk_oop_t AssertionFailureClass;
    crank_talk_oop_t MessageNotUnderstoodClass;

    // GUI
    crank_talk_oop_t PointClass;
    crank_talk_oop_t RectangleClass;
    crank_talk_oop_t FormClass;
    crank_talk_oop_t FormCanvasClass;
    crank_talk_oop_t OSWindowClass;
    crank_talk_oop_t OSWindowEventHandlerClass;
    
    crank_talk_oop_t OSEventClass;
    crank_talk_oop_t OSMouseEventClass;
    crank_talk_oop_t OSMouseButtonEventClass;
    crank_talk_oop_t OSMouseButtonPressEventClass;
    crank_talk_oop_t OSMouseButtonReleaseEventClass;
    crank_talk_oop_t OSMouseMoveEventClass;

    crank_talk_oop_t openWindowList;
};

struct crank_talk_VMContext_s
{
    crank_talk_VMContextRoots_t roots;
    crank_talk_heap_t heap;

    bool gcDisabled;
    crank_talk_dynarray_t markingStack;
};

CRANK_TALK_API crank_talk_VMContext_t *crank_talk_vm_createContext(void);
CRANK_TALK_API void crank_talk_vm_destroyContext(crank_talk_VMContext_t *context);


#endif //CRANK_TALK_CONTEXT_H
