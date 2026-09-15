#include "crank-talk/analysis-evaluation.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/dictionary.h"
#include "crank-talk/environment.h"
#include "crank-talk/errors.h"
#include "crank-talk/hir.h"
#include "crank-talk/message.h"
#include "crank-talk/parse-tree.h"
#include "crank-talk/stack-frame.h"
#include "crank-talk/string.h"
#include <stdlib.h>

CRANK_TALK_API crank_talk_oop_t
crank_talk_createTopLevelEvaluationEnvironment(crank_talk_VMContext_t *context, crank_talk_oop_t sourceCodeValue)
{
    crank_talk_oop_t emptyEnvironment = CrankAllocateValuesObjectWithClassName(context, EmptyEnvironment);
    
    crank_talk_oop_t systemEnvironmentEnvironmentValue = CrankAllocateValuesObjectWithClassName(context, SystemEnvironmentEnvironment);
    crank_talk_SystemEnvironmentEnvironment_t *systemEnvironmentEnvironment = (crank_talk_SystemEnvironmentEnvironment_t*)systemEnvironmentEnvironmentValue;
    systemEnvironmentEnvironment->parent = emptyEnvironment;
    systemEnvironmentEnvironment->systemEnvironment = context->roots.systemEnvironment;


    crank_talk_oop_t lexicalEnvironment = crank_talk_LexicalEnvironment_create(context, systemEnvironmentEnvironmentValue);

    if(!crank_talk_oop_isNil(sourceCodeValue))
    {
        crank_talk_SourceCode_t *sourceCode = (crank_talk_SourceCode_t*)sourceCodeValue;
        if(sourceCode->directory)
            crank_talk_LexicalEnvironment_setSymbolBinding(context, lexicalEnvironment, crank_talk_Symbol_internWithCString(context, "__FileDir__"), (crank_talk_oop_t)sourceCode->directory);
        if(sourceCode->name)
            crank_talk_LexicalEnvironment_setSymbolBinding(context, lexicalEnvironment, crank_talk_Symbol_internWithCString(context, "__FileName__"), (crank_talk_oop_t)sourceCode->name);
    }

    return lexicalEnvironment;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_createAnalysisAndEvaluationContext(crank_talk_VMContext_t *context, crank_talk_oop_t lexicalEnvironment)
{
    crank_talk_oop_t evaluationContextValue = CrankAllocateValuesObjectWithClassName(context, AnalysisAndEvaluationContext);
    crank_talk_AnalysisAndEvaluationContext_t *evaluationContext = (crank_talk_AnalysisAndEvaluationContext_t *)evaluationContextValue;
    evaluationContext->lexicalEnvironment = lexicalEnvironment;
    return evaluationContextValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_createTopLevelAnalysisAndEvaluationContext(crank_talk_VMContext_t *context, crank_talk_oop_t sourceCode)
{
    return crank_talk_createAnalysisAndEvaluationContext(context, crank_talk_createTopLevelEvaluationEnvironment(context, sourceCode));
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_ParseTreeAnalyzeAndEvaluate(crank_talk_VMContext_t *context, crank_talk_oop_t parseTreeNode, crank_talk_oop_t evaluationContext)
{
    return crank_talk_send1(context, context->roots.analyzeAndEvaluateWithSymbol, parseTreeNode, evaluationContext);
}

static crank_talk_oop_t
crank_talk_ParseTreeAssignmentNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeAssignmentNode_t *node = (crank_talk_ParseTreeAssignmentNode_t*)self;
    crank_talk_AnalysisAndEvaluationContext_t *evaluationContext = (crank_talk_AnalysisAndEvaluationContext_t*)evaluationContextValue;
   
    if(!crank_talk_oop_isKindOf(context, (crank_talk_oop_t)node->store, (crank_talk_oop_t)context->roots.ParseTreeIdentifierReferenceNodeClass))
        crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->store->sourcePosition, "Expected an identifier with the variable name to assign.");

    crank_talk_ParseTreeIdentifierReferenceNode_t *storeIdentifer = (crank_talk_ParseTreeIdentifierReferenceNode_t*)node->store;
    crank_talk_oop_t foundBinding = crank_talk_Environment_lookupSymbolRecursively(context, evaluationContext->lexicalEnvironment, (crank_talk_oop_t)storeIdentifer->symbol);
    if(crank_talk_oop_isNil(foundBinding))
        crank_talk_errorPrintfAtSourcePosition(context, (crank_talk_oop_t)node->super.sourcePosition, "Binding not found for symbol #" CRANK_TALK_STRING_FORMAT, CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)storeIdentifer->symbol));

    crank_talk_oop_t valueToStore = crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->value, evaluationContextValue);

    //// TODO: Use a message here.
    assert(crank_talk_oop_isKindOf(context, foundBinding, context->roots.LiteralVariableClass));
    ((crank_talk_LiteralVariable_t*)foundBinding)->value = valueToStore;
    return valueToStore;
}

static crank_talk_oop_t
crank_talk_ParseTreeBlockNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeBlockNode_t *node = (crank_talk_ParseTreeBlockNode_t*)self;
    crank_talk_AnalysisAndEvaluationContext_t *evaluationContext = (crank_talk_AnalysisAndEvaluationContext_t*)evaluationContextValue;

    struct {
        crank_talk_oop_t compiledFunctionValue;
        crank_talk_oop_t blockClosureValue;
    } gcFrame = {};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Compile the function
    gcFrame.compiledFunctionValue = CrankAllocateValuesObjectWithClassName(context, CompiledFunction);
    crank_talk_CompiledFunction_t *compiledFunction = (crank_talk_CompiledFunction_t*)gcFrame.compiledFunctionValue;
    compiledFunction->argumentCount = crank_talk_oop_encodeSmallInteger(crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->arguments));
    compiledFunction->isBlock = crank_talk_oop_fromBoolean(context, true);
    compiledFunction->definitionEnvironment = evaluationContext->lexicalEnvironment;
    compiledFunction->definitionArguments = (crank_talk_Array_t*)node->arguments;
    compiledFunction->definitionBody = (crank_talk_oop_t)node->body;
    crank_talk_CompiledFunction_ensureAnalysis(context, compiledFunction);

    // Create the captureless block closure
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)((crank_talk_HIRFunction_t*)compiledFunction->hirFunction)->captures) == 0);
    gcFrame.blockClosureValue = CrankAllocateValuesObjectWithClassName(context, BlockClosure);
    crank_talk_BlockClosure_t *blockClosure = (crank_talk_BlockClosure_t *)gcFrame.blockClosureValue;
    blockClosure->captureVector = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    blockClosure->function = compiledFunction;

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);

    return gcFrame.blockClosureValue;
}

static crank_talk_oop_t
crank_talk_ParseTreeLiteralVariableNode_analyzeAndEvaluateIdentifierReferenceWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    //crank_talk_oop_t identifierReference = arguments[1];
    //crank_talk_oop_t evaluationContextValue = arguments[2];

    crank_talk_LiteralVariable_t *literalVariable = (crank_talk_LiteralVariable_t*)self;
    return literalVariable->value;
}


static crank_talk_oop_t
crank_talk_ParseTreeIdentifierReferenceNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeIdentifierReferenceNode_t *node = (crank_talk_ParseTreeIdentifierReferenceNode_t*)self;
    crank_talk_AnalysisAndEvaluationContext_t *evaluationContext = (crank_talk_AnalysisAndEvaluationContext_t*)evaluationContextValue;
    crank_talk_oop_t foundBinding = crank_talk_Environment_lookupSymbolRecursively(context, evaluationContext->lexicalEnvironment, (crank_talk_oop_t)node->symbol);
    if(crank_talk_oop_isNil(foundBinding))
        crank_talk_errorPrintfAtSourcePosition(context, (crank_talk_oop_t)node->super.sourcePosition, "Binding not found for symbol #" CRANK_TALK_STRING_FORMAT, CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)node->symbol));

    return crank_talk_send2(context, context->roots.analyzeAndEvaluateIdentifierReferenceWithSymbol, foundBinding, self, evaluationContextValue);
}

static crank_talk_oop_t
crank_talk_ParseTreeLiteralNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_ParseTreeLiteralNode_t *node = (crank_talk_ParseTreeLiteralNode_t*)self;
    return node->value;
}

static crank_talk_oop_t
crank_talk_ParseTreeNilReference_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)arguments;
    (void)argumentCount;
    assert(argumentCount >= 2);
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_ParseTreeFalseReference_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)arguments;
    (void)argumentCount;
    assert(argumentCount >= 2);
    return context->roots.falseValue;
}

static crank_talk_oop_t
crank_talk_ParseTreeTrueReference_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)arguments;
    (void)argumentCount;
    assert(argumentCount >= 2);
    return context->roots.trueValue;
}

static crank_talk_oop_t
crank_talk_ParseTreeSelfReference_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_ParseTreeSelfReferenceNode_t *node = (crank_talk_ParseTreeSelfReferenceNode_t*)arguments[0];

    crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->super.super.sourcePosition, "Invalid context for self.");
    abort();
}

static crank_talk_oop_t
crank_talk_ParseTreeSuperReference_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_ParseTreeSuperReferenceNode_t *node = (crank_talk_ParseTreeSuperReferenceNode_t*)arguments[0];

    crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->super.super.sourcePosition, "Invalid context for super.");
    abort();
}


static crank_talk_oop_t
crank_talk_ParseTreeThisContextReference_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    //crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_AnalysisAndEvaluationContext_t *evaluationContext = (crank_talk_AnalysisAndEvaluationContext_t*)evaluationContextValue;

    crank_talk_oop_t scriptContextValue = CrankAllocateValuesObjectWithClassName(context, ScriptContext);
    crank_talk_ScriptContext_t *scriptContext = (crank_talk_ScriptContext_t *)scriptContextValue;
    scriptContext->lexicalEnvironment = evaluationContext->lexicalEnvironment;
    return scriptContextValue;
}

static crank_talk_oop_t
crank_talk_ParseTreeSequenceNodeWithLocals_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeSequenceNode_t *node = (crank_talk_ParseTreeSequenceNode_t*)self;
    crank_talk_AnalysisAndEvaluationContext_t *evaluationContext = (crank_talk_AnalysisAndEvaluationContext_t*)evaluationContextValue;

    struct {
        crank_talk_oop_t oldLexicalEnvironment;
        crank_talk_oop_t result;
    } gcFrame = {
        .oldLexicalEnvironment = evaluationContext->lexicalEnvironment,
        .result = crank_talk_oop_nil()
    };
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.oldLexicalEnvironment = evaluationContext->lexicalEnvironment;
    evaluationContext->lexicalEnvironment = crank_talk_LexicalEnvironment_create(context, evaluationContext->lexicalEnvironment);

    // Define the local variables.
    size_t localVariableCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->localVariables);
    crank_talk_oop_t *locals = crank_talk_oop_getSlotsData((crank_talk_oop_t)node->localVariables);
    for(size_t i = 0; i < localVariableCount; ++i)
        crank_talk_ParseTreeAnalyzeAndEvaluate(context, locals[i], evaluationContextValue);

    // Evaluate the expressions.
    size_t expressionCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->expressions);
    crank_talk_oop_t *expressions = crank_talk_oop_getSlotsData((crank_talk_oop_t)node->expressions);
    for(size_t i = 0; i < expressionCount; ++i)
        gcFrame.result = crank_talk_ParseTreeAnalyzeAndEvaluate(context, expressions[i], evaluationContextValue);

    evaluationContext->lexicalEnvironment = gcFrame.oldLexicalEnvironment;
    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return gcFrame.result;
}

static crank_talk_oop_t
crank_talk_ParseTreeCascadedMessageNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];

    crank_talk_ParseTreeMessageCascadeNode_t *node = (crank_talk_ParseTreeMessageCascadeNode_t*)self;

    crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->super.sourcePosition, "Invalid location for a cascaded message send.");
    abort();
}

static crank_talk_oop_t
crank_talk_ParseTreeMessageCascadeNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContext = arguments[1];

    crank_talk_ParseTreeMessageCascadeNode_t *node = (crank_talk_ParseTreeMessageCascadeNode_t*)self;
   
    struct {
        crank_talk_oop_t receiverValue;
        crank_talk_oop_t receiverNode;
        crank_talk_oop_t result;
    } gcFrame = {0};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.receiverValue = crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->receiver, evaluationContext);
    gcFrame.result = gcFrame.receiverValue;

    // Create the receiver literal node value.
    gcFrame.receiverNode = CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    crank_talk_ParseTreeLiteralNode_t *receiverNode = (crank_talk_ParseTreeLiteralNode_t *)gcFrame.receiverNode;
    receiverNode->super.sourcePosition = node->receiver->sourcePosition;
    receiverNode->value = gcFrame.receiverValue;

    // Expand and evaluate the cascaded messages.
    size_t cascadeMessageCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->cascadedMessages);
    for(size_t i = 0; i < cascadeMessageCount; ++i)
    {
        crank_talk_oop_t cascadedMessageValue = node->cascadedMessages->elements[i];
        crank_talk_ParseTreeCascadedMessageNode_t *cascadedMessage = (crank_talk_ParseTreeCascadedMessageNode_t*)cascadedMessageValue;
        
        crank_talk_oop_t expandedCascadedMessage = crank_talk_ParseTreeCascadedMessageNode_asMessageSendWithReceiver(context, cascadedMessage, gcFrame.receiverNode);
        gcFrame.result = crank_talk_ParseTreeAnalyzeAndEvaluate(context, expandedCascadedMessage, evaluationContext);
    }    

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return gcFrame.result;
}

static crank_talk_oop_t
crank_talk_ParseTreeMessageSendNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContext = arguments[1];

    crank_talk_ParseTreeMessageSendNode_t *node = (crank_talk_ParseTreeMessageSendNode_t*)self;
   
    struct {
        crank_talk_oop_t selector;
        crank_talk_oop_t callArguments[CRANK_TALK_MAX_CALL_ARGUMENT_COUNT];
        crank_talk_oop_t result;
    } gcFrame = {
        .result = crank_talk_oop_nil()
    };
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.selector = crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->selector, evaluationContext);
    if(!crank_talk_oop_isKindOf(context, gcFrame.selector, context->roots.SymbolClass))
        crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->selector->sourcePosition, "Expected a symbol as the message selector.");

    // Is this a macro message send?
    if(crank_talk_IdentityDictionary_includesKey(context->roots.messageSendMacroDictionary, gcFrame.selector))
    {
        crank_talk_oop_t messageMacro = crank_talk_IdentityDictionary_at(context->roots.messageSendMacroDictionary, gcFrame.selector);
        crank_talk_oop_t expandedMacro = crank_talk_apply1(context, messageMacro, arguments[0]);
        CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
        return (crank_talk_oop_t)crank_talk_ParseTreeAnalyzeAndEvaluate(context, expandedMacro, evaluationContext);
    }

    size_t messageArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->arguments);
    assert(messageArgumentCount < CRANK_TALK_MAX_CALL_ARGUMENT_COUNT);
    gcFrame.callArguments[0] = crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->receiver, evaluationContext);
    for(size_t i = 0; i < messageArgumentCount; ++i)
        gcFrame.callArguments[i + 1] = crank_talk_ParseTreeAnalyzeAndEvaluate(context, node->arguments->elements[i], evaluationContext);
    
    gcFrame.result = crank_talk_send(context, gcFrame.selector, messageArgumentCount + 1, gcFrame.callArguments);

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return gcFrame.result;
}

static crank_talk_oop_t
crank_talk_ParseTreeSequenceNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContext = arguments[1];

    crank_talk_ParseTreeSequenceNode_t *node = (crank_talk_ParseTreeSequenceNode_t*)self;
    if(crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->localVariables) != 0)
        return crank_talk_ParseTreeSequenceNodeWithLocals_analyzeAndEvaluateWith(context, closure, argumentCount, arguments);
   
    struct {
        crank_talk_oop_t result;
    } gcFrame = {
        .result = crank_talk_oop_nil()
    };
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    size_t expressionCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->expressions);
    crank_talk_oop_t *expressions = crank_talk_oop_getSlotsData((crank_talk_oop_t)node->expressions);
    for(size_t i = 0; i < expressionCount; ++i)
        gcFrame.result = crank_talk_ParseTreeAnalyzeAndEvaluate(context, expressions[i], evaluationContext);

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return gcFrame.result;
}

static crank_talk_oop_t
crank_talk_ParseTreeArrayNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContext = arguments[1];

    crank_talk_ParseTreeArrayNode_t *node = (crank_talk_ParseTreeArrayNode_t*)self;
   
    struct {
        crank_talk_Array_t *result;
    } gcFrame = {0};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    size_t elementCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->elements);
    crank_talk_oop_t *elements = crank_talk_oop_getSlotsData((crank_talk_oop_t)node->elements);
    gcFrame.result = (crank_talk_Array_t*)crank_talk_Array_create(context, elementCount);
    for(size_t i = 0; i < elementCount; ++i)
        gcFrame.result->elements[i] = crank_talk_ParseTreeAnalyzeAndEvaluate(context, elements[i], evaluationContext);

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return (crank_talk_oop_t)gcFrame.result;
}

static crank_talk_oop_t
crank_talk_ParseTreeReturnNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];

    crank_talk_ParseTreeReturnNode_t *node = (crank_talk_ParseTreeReturnNode_t*)self;

    crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->super.sourcePosition, "Invalid location for a return statement.");
    abort();
}

static crank_talk_oop_t
crank_talk_ParseTreeAddMethodNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeAddMethodNode_t *node = (crank_talk_ParseTreeAddMethodNode_t*)self;

    struct {
        crank_talk_oop_t owner;
        crank_talk_oop_t method;
    } gcFrame = {};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Evaluate the owner.
    gcFrame.owner = crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->owner, evaluationContextValue);
    
    // Is the owner a behavior
    if(!crank_talk_oop_isKindOf(context, gcFrame.owner, context->roots.BehaviorClass))
        crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->owner->sourcePosition, "Expected a behavior.");

    // Set the owner in the method node.
    if(crank_talk_oop_isKindOf(context, (crank_talk_oop_t)node->method, context->roots.ParseTreeMethodDefinitionNodeClass))
    {
        crank_talk_ParseTreeMethodDefinitionNode_t *methodNode = (crank_talk_ParseTreeMethodDefinitionNode_t *)node->method;
        methodNode->owner = gcFrame.owner;
    }

    // Evaluate the method
    gcFrame.method = crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->method, evaluationContextValue);
    crank_talk_CompiledFunction_t *method = (crank_talk_CompiledFunction_t *)gcFrame.method;

    // Add the method to the owner.
    assert(crank_talk_oop_isKindOf(context, gcFrame.owner, context->roots.BehaviorClass));
    crank_talk_Behavior_withSelectorSymbolSetMethod(context, gcFrame.owner, method->name, gcFrame.method);

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);

    return (crank_talk_oop_t)gcFrame.method;
}

static crank_talk_oop_t
crank_talk_ParseTreeMethodDefinitionNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeMethodDefinitionNode_t *node = (crank_talk_ParseTreeMethodDefinitionNode_t*)self;
    crank_talk_AnalysisAndEvaluationContext_t *evaluationContext = (crank_talk_AnalysisAndEvaluationContext_t*)evaluationContextValue;

    struct {
        crank_talk_oop_t compiledFunctionValue;
    } gcFrame = {};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Compile the function
    gcFrame.compiledFunctionValue = CrankAllocateValuesObjectWithClassName(context, CompiledFunction);
    crank_talk_CompiledFunction_t *compiledFunction = (crank_talk_CompiledFunction_t*)gcFrame.compiledFunctionValue;
    compiledFunction->name = (crank_talk_oop_t)node->selector;
    compiledFunction->owner = node->owner;
    compiledFunction->argumentCount = crank_talk_oop_encodeSmallInteger(crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->arguments));
    compiledFunction->isBlock = crank_talk_oop_fromBoolean(context, false);
    compiledFunction->definitionEnvironment = evaluationContext->lexicalEnvironment;
    compiledFunction->definitionArguments = (crank_talk_Array_t*)node->arguments;
    compiledFunction->definitionBody = (crank_talk_oop_t)node->body;
    crank_talk_CompiledFunction_ensureAnalysis(context, compiledFunction);

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);

    return (crank_talk_oop_t)compiledFunction;
}

static crank_talk_oop_t
crank_talk_ParseTreeVariableDefinitionNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeVariableDefinitionNode_t *node = (crank_talk_ParseTreeVariableDefinitionNode_t*)self;
    crank_talk_AnalysisAndEvaluationContext_t *evaluationContext = (crank_talk_AnalysisAndEvaluationContext_t*)evaluationContextValue;

    crank_talk_LexicalEnvironment_setNewSymbolBinding(context, evaluationContext->lexicalEnvironment, (crank_talk_oop_t)node->name, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_ParseTreeCullNodeNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_AnalysisAndEvaluationContext_t *evaluationContext = (crank_talk_AnalysisAndEvaluationContext_t*)evaluationContextValue;

    struct {
        crank_talk_oop_t oldLexicalEnvironment;
        crank_talk_oop_t result;
    } gcFrame = {
        .oldLexicalEnvironment = evaluationContext->lexicalEnvironment,
        .result = crank_talk_oop_nil()
    };
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.oldLexicalEnvironment = evaluationContext->lexicalEnvironment;
    evaluationContext->lexicalEnvironment = crank_talk_LexicalEnvironment_create(context, evaluationContext->lexicalEnvironment);

    // Make sure we have an inlinable block
    crank_talk_ParseTreeCullBlockNode_t *node = (crank_talk_ParseTreeCullBlockNode_t*)self;
    if(!crank_talk_oop_isKindOf(context, (crank_talk_oop_t)node->blockExpression, context->roots.ParseTreeBlockNodeClass))
        crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->blockExpression->sourcePosition, "Expected an inlinable block expression.");

    crank_talk_ParseTreeBlockNode_t *blockNode = (crank_talk_ParseTreeBlockNode_t*)node->blockExpression;

    // Check the block argument counts.
    size_t blockArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)blockNode->arguments);
    size_t cullArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->arguments);
    if(blockArgumentCount > cullArgumentCount)
        crank_talk_errorPrintfAtSourcePosition(context, (crank_talk_oop_t)node->super.sourcePosition, "Inlinable block expects %d instead of %d optinal arguments", (int)blockArgumentCount, (int)cullArgumentCount);

    // Set the block arguments.
    crank_talk_oop_t *blockArguments = blockNode->arguments->elements;
    crank_talk_oop_t *cullArguments = node->arguments->elements;
    for(size_t i = 0; i < blockArgumentCount; ++i)
    {
        crank_talk_oop_t argumentDefinitionValue = blockArguments[i];

        if(!crank_talk_oop_isKindOf(context, argumentDefinitionValue, context->roots.ParseTreeArgumentDefinitionNodeClass))
            crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)((crank_talk_ParseTreeNode_t*)argumentDefinitionValue)->sourcePosition, "Expected an argument definition node.");

        crank_talk_ParseTreeArgumentDefinitionNode_t *argumentDefinition = (crank_talk_ParseTreeArgumentDefinitionNode_t*)argumentDefinitionValue;
        crank_talk_LexicalEnvironment_setNewSymbolBinding(context, evaluationContext->lexicalEnvironment, (crank_talk_oop_t)argumentDefinition->name, cullArguments[i], (crank_talk_oop_t)node->super.sourcePosition);
    }
   
    // Evaluate the block body
    gcFrame.result = crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)blockNode->body, evaluationContextValue);

    // Finish
    evaluationContext->lexicalEnvironment = gcFrame.oldLexicalEnvironment;
    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return gcFrame.result;
}

static crank_talk_oop_t
crank_talk_ParseTreeIfSelectionNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeIfSelectionNode_t *node = (crank_talk_ParseTreeIfSelectionNode_t*)self;

    crank_talk_oop_t condition = crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->condition, evaluationContextValue);
    if(condition == context->roots.trueValue)
    {
        if(node->trueExpression)
            return crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->trueExpression, evaluationContextValue);
        else
            return crank_talk_oop_nil();
    }
    else if(condition == context->roots.falseValue)
    {
        if(node->falseExpression)
            return crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->falseExpression, evaluationContextValue);
        else
            return crank_talk_oop_nil();
    }
    else
    {
        crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->condition->sourcePosition, "If condition must be true or false.");
        abort();
    }
}

static crank_talk_oop_t
crank_talk_ParseTreeWhileNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeWhileNode_t *node = (crank_talk_ParseTreeWhileNode_t*)self;

    while(!node->condition || crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->condition, evaluationContextValue) == context->roots.trueValue)
    {
        if(node->bodyExpression)
            crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->bodyExpression, evaluationContextValue);
    }

    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_ParseTreeDoWhileNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContextValue = arguments[1];

    crank_talk_ParseTreeDoWhileNode_t *node = (crank_talk_ParseTreeDoWhileNode_t*)self;

    do
    {
        if(node->bodyExpression)
            crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->bodyExpression, evaluationContextValue);
    } while (!node->condition || crank_talk_ParseTreeAnalyzeAndEvaluate(context, (crank_talk_oop_t)node->condition, evaluationContextValue) == context->roots.trueValue);

    return crank_talk_oop_nil();
}

void
crank_talk_registerAnalysisAndEvaluationPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LiteralVariableClass, "analyzeAndEvaluateIdentifierReference:with:", 3, crank_talk_ParseTreeLiteralVariableNode_analyzeAndEvaluateIdentifierReferenceWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeAssignmentNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeAssignmentNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeBlockNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeBlockNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeIdentifierReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeIdentifierReferenceNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeLiteralNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeLiteralNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeNilReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeNilReference_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeFalseReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeFalseReference_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeTrueReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeTrueReference_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSelfReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeSelfReference_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSuperReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeSuperReference_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeThisContextReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeThisContextReference_analyzeAndEvaluateWith);
    
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeCascadedMessageNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeCascadedMessageNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageCascadeNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeMessageCascadeNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageSendNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeMessageSendNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSequenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeSequenceNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeArrayNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeArrayNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeReturnNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeReturnNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeAddMethodNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeAddMethodNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMethodDefinitionNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeMethodDefinitionNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeVariableDefinitionNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeVariableDefinitionNode_analyzeAndEvaluateWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeCullBlockNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeCullNodeNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeIfSelectionNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeIfSelectionNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeWhileNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeWhileNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeDoWhileNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeDoWhileNode_analyzeAndEvaluateWith);
}
