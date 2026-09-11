#include "crank-talk/analysis-evaluation.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/environment.h"
#include "crank-talk/errors.h"
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
    gcFrame.blockClosureValue = CrankAllocateValuesObjectWithClassName(context, BlockClosure);
    crank_talk_BlockClosure_t *blockClosure = (crank_talk_BlockClosure_t *)gcFrame.blockClosureValue;
    blockClosure->captureVector = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    blockClosure->function = compiledFunction;

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);

    return gcFrame.blockClosureValue;
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

    // TODO: Use a message here.
    assert(crank_talk_oop_isKindOf(context, foundBinding, context->roots.LiteralVariableClass));
    return ((crank_talk_LiteralVariable_t*)foundBinding)->value;
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
crank_talk_ParseTreeCascadedMessageNode_asMessageSendWithReceiver(crank_talk_VMContext_t *context, crank_talk_ParseTreeCascadedMessageNode_t *cascadedMessage, crank_talk_oop_t receiver)
{
    crank_talk_oop_t messageSendNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeMessageSendNode);
    crank_talk_ParseTreeMessageSendNode_t *messageSendNode = (crank_talk_ParseTreeMessageSendNode_t *)messageSendNodeValue;
    messageSendNode->super.sourcePosition = cascadedMessage->super.sourcePosition;
    messageSendNode->receiver = (crank_talk_ParseTreeNode_t*)receiver;
    messageSendNode->selector = cascadedMessage->selector;
    messageSendNode->arguments = cascadedMessage->arguments;
    return messageSendNodeValue;
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

void
crank_talk_registerAnalysisAndEvaluationPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeAssignmentNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeAssignmentNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeBlockNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeBlockNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeIdentifierReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeIdentifierReferenceNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeLiteralNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeLiteralNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeNilReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeNilReference_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeFalseReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeFalseReference_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeTrueReferenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeTrueReference_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeCascadedMessageNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeCascadedMessageNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageCascadeNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeMessageCascadeNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageSendNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeMessageSendNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSequenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeSequenceNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeArrayNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeArrayNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeReturnNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeReturnNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeVariableDefinitionNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeVariableDefinitionNode_analyzeAndEvaluateWith);
}
