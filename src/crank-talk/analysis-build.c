#include "crank-talk/analysis-build.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/dictionary.h"
#include "crank-talk/environment.h"
#include "crank-talk/errors.h"
#include "crank-talk/function.h"
#include "crank-talk/hir.h"
#include "crank-talk/message.h"
#include "crank-talk/parse-tree.h"
#include "crank-talk/stack-frame.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

CRANK_TALK_API crank_talk_oop_t
crank_talk_createAnalysisAndBuildContext(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder)
{
    crank_talk_oop_t buildContextValue = CrankAllocateValuesObjectWithClassName(context, AnalysisAndBuildContext);
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;
    buildContext->builder = builder;
    return buildContextValue;
}

CRANK_TALK_API crank_talk_HIRValue_t*
crank_talk_ParseTreeAnalyzeAndBuild(crank_talk_VMContext_t *context, crank_talk_oop_t parseTreeNode, crank_talk_oop_t buildContext)
{
    return (crank_talk_HIRValue_t*)crank_talk_send1(context, context->roots.analyzeAndBuildWithSymbol, parseTreeNode, buildContext);
}

static crank_talk_oop_t
crank_talk_LiteralVariable_analyzeAndBuildAssignmentWithWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 4);
    crank_talk_oop_t self = arguments[0];
    crank_talk_ParseTreeAssignmentNode_t *assignmentNode =(crank_talk_ParseTreeAssignmentNode_t*)arguments[1];
    crank_talk_HIRValue_t *assignedValue = (crank_talk_HIRValue_t*)arguments[2];
    crank_talk_oop_t buildContextValue = arguments[3];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_HIRValue_t *literalVariableValue = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(context, self, (crank_talk_oop_t)assignmentNode->super.sourcePosition);
    crank_talk_HIRBuilder_variableStore(context, buildContext->builder, literalVariableValue, assignedValue, (crank_talk_oop_t)assignmentNode->super.sourcePosition);
    return (crank_talk_oop_t)assignedValue;
}

static crank_talk_oop_t
crank_talk_HIRVariable_analyzeAndBuildAssignmentWithWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 4);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t assignmentNode = arguments[1];
    crank_talk_oop_t assignedValue = arguments[2];
    crank_talk_oop_t buildContextValue = arguments[3];

    crank_talk_HIRVariable_t *hirVariable = (crank_talk_HIRVariable_t *)self;
    return crank_talk_send3(context, context->roots.analyzeAndBuildAssignmentWithValueWithSymbol, hirVariable->value, assignmentNode, assignedValue, buildContextValue);
}

static crank_talk_oop_t
crank_talk_HIRAlloca_analyzeAndBuildAssignmentWithWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 4);
    crank_talk_HIRValue_t* self = (crank_talk_HIRValue_t*)arguments[0];
    crank_talk_ParseTreeAssignmentNode_t *assignmentNode =(crank_talk_ParseTreeAssignmentNode_t*)arguments[1];
    crank_talk_HIRValue_t *assignedValue = (crank_talk_HIRValue_t*)arguments[2];
    crank_talk_oop_t buildContextValue = arguments[3];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_HIRBuilder_variableStore(context, buildContext->builder, self, assignedValue, (crank_talk_oop_t)assignmentNode->super.sourcePosition);
    return (crank_talk_oop_t)assignedValue;
}

static crank_talk_oop_t
crank_talk_ParseTreeAssignmentNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t buildContextValue = arguments[1];

    crank_talk_ParseTreeAssignmentNode_t *node = (crank_talk_ParseTreeAssignmentNode_t*)self;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_HIRValue_t *assignedValue = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->value, buildContextValue);

    if(!crank_talk_oop_isKindOf(context, (crank_talk_oop_t)node->store, (crank_talk_oop_t)context->roots.ParseTreeIdentifierReferenceNodeClass))
        crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->store->sourcePosition, "Expected an identifier with the variable name to assign.");

    crank_talk_ParseTreeIdentifierReferenceNode_t *storeIdentifer = (crank_talk_ParseTreeIdentifierReferenceNode_t*)node->store;
    crank_talk_oop_t foundBinding = crank_talk_Environment_lookupSymbolRecursively(context, buildContext->builder->lexicalEnvironment, (crank_talk_oop_t)storeIdentifer->symbol);
    if(crank_talk_oop_isNil(foundBinding))
        crank_talk_errorPrintfAtSourcePosition(context, (crank_talk_oop_t)node->super.sourcePosition, "Binding not found for symbol #" CRANK_TALK_STRING_FORMAT, CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)storeIdentifer->symbol));

    return crank_talk_send3(context, context->roots.analyzeAndBuildAssignmentWithValueWithSymbol, foundBinding, self, (crank_talk_oop_t)assignedValue, buildContextValue);
}

static crank_talk_oop_t
crank_talk_ParseTreeBlockNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t buildContextValue = arguments[1];

    crank_talk_ParseTreeBlockNode_t *node = (crank_talk_ParseTreeBlockNode_t*)self;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

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
    compiledFunction->definitionEnvironment = buildContext->builder->lexicalEnvironment;
    compiledFunction->definitionArguments = (crank_talk_Array_t*)node->arguments;
    compiledFunction->definitionBody = (crank_talk_oop_t)node->body;
    crank_talk_CompiledFunction_ensureAnalysis(context, compiledFunction);

    crank_talk_HIRFunction_t *hirFunction = (crank_talk_HIRFunction_t *)compiledFunction->hirFunction;
    size_t captureCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)hirFunction->captures);
    if(captureCount == 0)
    {
        // FIXME: Create a closure with the captured values.
        // Create the captureless block closure
        gcFrame.blockClosureValue = CrankAllocateValuesObjectWithClassName(context, BlockClosure);
        crank_talk_BlockClosure_t *blockClosure = (crank_talk_BlockClosure_t *)gcFrame.blockClosureValue;
        blockClosure->captureVector = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
        blockClosure->function = compiledFunction;

        CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
        return crank_talk_HIRConstantLiteralValue_create(context, gcFrame.blockClosureValue, (crank_talk_oop_t)node->super.sourcePosition);
    }
    else
    {
        printf("TODO: instantiate the block closure with the captured values\n");
        abort();
    }

}

static crank_talk_oop_t
crank_talk_LiteralVariable_analyzeAndBuildIdentifierReferenceWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t referenceNode = arguments[1];
    crank_talk_oop_t buildContextValue = arguments[2];

    crank_talk_ParseTreeIdentifierReferenceNode_t *node = (crank_talk_ParseTreeIdentifierReferenceNode_t*)referenceNode;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_HIRValue_t *variableValue = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(context, self, (crank_talk_oop_t)node->super.sourcePosition);
    return crank_talk_HIRBuilder_variableLoad(context, buildContext->builder, variableValue, (crank_talk_oop_t)node->super.sourcePosition);
}

static crank_talk_oop_t
crank_talk_HIRVariable_analyzeAndBuildIdentifierReferenceWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t referenceNode = arguments[1];
    crank_talk_oop_t buildContextValue = arguments[2];

    crank_talk_HIRVariable_t *hirVariable = (crank_talk_HIRVariable_t *)self;
    return crank_talk_send2(context, context->roots.analyzeAndBuildIdentifierReferenceWithSymbol, hirVariable->value, referenceNode, buildContextValue);
}

static crank_talk_oop_t
crank_talk_HIRValue_analyzeAndBuildIdentifierReferenceWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    //crank_talk_oop_t referenceNode = arguments[1];
    //crank_talk_oop_t buildContextValue = arguments[2];

    return self;
}

static crank_talk_oop_t
crank_talk_HIRAllocaInstruction_analyzeAndBuildIdentifierReferenceWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_HIRValue_t *self = (crank_talk_HIRValue_t*)arguments[0];
    crank_talk_oop_t referenceNode = arguments[1];
    crank_talk_oop_t buildContextValue = arguments[2];
    
    crank_talk_ParseTreeIdentifierReferenceNode_t *node = (crank_talk_ParseTreeIdentifierReferenceNode_t*)referenceNode;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    return crank_talk_HIRBuilder_variableLoad(context, buildContext->builder, self, (crank_talk_oop_t)node->super.sourcePosition);
}

static crank_talk_oop_t
crank_talk_ParseTreeIdentifierReferenceNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t buildContextValue = arguments[1];

    crank_talk_ParseTreeIdentifierReferenceNode_t *node = (crank_talk_ParseTreeIdentifierReferenceNode_t*)self;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_oop_t foundBinding = crank_talk_Environment_lookupSymbolRecursively(context, buildContext->builder->lexicalEnvironment, (crank_talk_oop_t)node->symbol);
    if(crank_talk_oop_isNil(foundBinding))
        crank_talk_errorPrintfAtSourcePosition(context, (crank_talk_oop_t)node->super.sourcePosition, "Binding not found for symbol #" CRANK_TALK_STRING_FORMAT, CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)node->symbol));
    
    return crank_talk_send2(context, context->roots.analyzeAndBuildIdentifierReferenceWithSymbol, foundBinding, self, buildContextValue);
}

static crank_talk_oop_t
crank_talk_ParseTreeLiteralNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeLiteralNode_t *node = (crank_talk_ParseTreeLiteralNode_t*)arguments[0];

    // FIXME: Use a message here-
    if(crank_talk_oop_isKindOf(context, node->value, context->roots.HIRValueClass))
        return node->value;
    
    return crank_talk_HIRConstantLiteralValue_create(context, node->value, (crank_talk_oop_t)node->super.sourcePosition);
}

static crank_talk_oop_t
crank_talk_ParseTreeNilReferenceNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeNode_t *node = (crank_talk_ParseTreeNode_t*)arguments[0];
    return crank_talk_HIRConstantLiteralValue_create(context, crank_talk_oop_nil(), (crank_talk_oop_t)node->sourcePosition);
}

static crank_talk_oop_t
crank_talk_ParseTreeFalseReferenceNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeNode_t *node = (crank_talk_ParseTreeNode_t*)arguments[0];
    return crank_talk_HIRConstantLiteralValue_create(context, context->roots.falseValue, (crank_talk_oop_t)node->sourcePosition);
}

static crank_talk_oop_t
crank_talk_ParseTreeTrueReferenceNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeNode_t *node = (crank_talk_ParseTreeNode_t*)arguments[0];
    return crank_talk_HIRConstantLiteralValue_create(context, context->roots.trueValue, (crank_talk_oop_t)node->sourcePosition);
}

static crank_talk_oop_t
crank_talk_ParseTreeMessageCascadeNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    assert(argumentCount >= 2);
    crank_talk_ParseTreeMessageCascadeNode_t *node = (crank_talk_ParseTreeMessageCascadeNode_t*)arguments[0];
    crank_talk_oop_t buildContextValue = arguments[1];

    struct {
        crank_talk_HIRValue_t *receiver;
        crank_talk_ParseTreeLiteralNode_t *receiverNode;
        crank_talk_HIRValue_t *result;
    } gcFrame = {0};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Evaluate the receiver and make the 
    gcFrame.receiver = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->receiver, buildContextValue);
    gcFrame.receiverNode = (crank_talk_ParseTreeLiteralNode_t*)CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    gcFrame.receiverNode->super.sourcePosition = node->receiver->sourcePosition;
    gcFrame.receiverNode->value = (crank_talk_oop_t)gcFrame.receiver;

    //crank_talk_ParseTreeLiteralNode_t *node = (crank_talk_ParseTreeLiteralNode_t*)arguments[0];
    gcFrame.result = gcFrame.receiver;

    size_t cascadedMessageCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->cascadedMessages);
    for(size_t i = 0; i < cascadedMessageCount; ++i)
    {
        crank_talk_oop_t cascadedMessageValue = node->cascadedMessages->elements[i];
        crank_talk_ParseTreeCascadedMessageNode_t *cascadedMessage = (crank_talk_ParseTreeCascadedMessageNode_t*)cascadedMessageValue;

        crank_talk_oop_t expandedCascadedMessage = crank_talk_ParseTreeCascadedMessageNode_asMessageSendWithReceiver(context, cascadedMessage, (crank_talk_oop_t)gcFrame.receiverNode);
        gcFrame.result = crank_talk_ParseTreeAnalyzeAndBuild(context, expandedCascadedMessage, buildContextValue);
    }

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return (crank_talk_oop_t)gcFrame.result;
}

static crank_talk_oop_t
crank_talk_ParseTreeMessageSendNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    assert(argumentCount >= 2);
    crank_talk_ParseTreeMessageSendNode_t *node = (crank_talk_ParseTreeMessageSendNode_t*)arguments[0];
    crank_talk_oop_t buildContextValue = arguments[1];
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;

    struct {
        crank_talk_HIRValue_t *selector;
        crank_talk_oop_t selectorValue;
        crank_talk_HIRValue_t *receiver;
        crank_talk_Array_t *argumentsWithReceiver;
    } gcFrame = {0};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Fetch the selector value.
    gcFrame.selector = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->selector, buildContextValue);
    assert(crank_talk_oop_isKindOf(context, (crank_talk_oop_t)gcFrame.selector, context->roots.HIRConstantLiteralValueClass));
    gcFrame.selectorValue = ((crank_talk_HIRConstantLiteralValue_t*)gcFrame.selector)->value;

    // Is this a macro message send?
    if(crank_talk_IdentityDictionary_includesKey(context->roots.messageSendMacroDictionary, gcFrame.selectorValue))
    {
        crank_talk_oop_t messageMacro = crank_talk_IdentityDictionary_at(context->roots.messageSendMacroDictionary, gcFrame.selectorValue);
        crank_talk_oop_t expandedMacro = crank_talk_apply1(context, messageMacro, arguments[0]);
        CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
        return (crank_talk_oop_t)crank_talk_ParseTreeAnalyzeAndBuild(context, expandedMacro, buildContextValue);
    }

    gcFrame.receiver = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->receiver, buildContextValue);
    size_t messageArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->arguments);
    gcFrame.argumentsWithReceiver = (crank_talk_Array_t*)crank_talk_Array_create(context, 1 + messageArgumentCount);
    gcFrame.argumentsWithReceiver->elements[0] = (crank_talk_oop_t)gcFrame.receiver;
    for(size_t i = 0; i < messageArgumentCount; ++i)
        gcFrame.argumentsWithReceiver->elements[i + 1] = (crank_talk_oop_t)crank_talk_ParseTreeAnalyzeAndBuild(context, node->arguments->elements[i], buildContextValue);

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return crank_talk_HIRBuilder_send(context, buildContext->builder, gcFrame.selector, gcFrame.argumentsWithReceiver, (crank_talk_oop_t)node->super.sourcePosition);
}

static crank_talk_oop_t
crank_talk_ParseTreeSequenceNodeWithLocals_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeSequenceNode_t *node = (crank_talk_ParseTreeSequenceNode_t*)arguments[0];
    crank_talk_oop_t buildContextValue = arguments[1];
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;

    struct {
        crank_talk_oop_t oldLexicalEnvironment;
        crank_talk_HIRValue_t *result;
    } gcFrame = {0};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.oldLexicalEnvironment = buildContext->builder->lexicalEnvironment;
    buildContext->builder->lexicalEnvironment = crank_talk_LexicalEnvironment_create(context, buildContext->builder->lexicalEnvironment);

    // Build the local variables
    size_t localVariableCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->localVariables);
    for(size_t i = 0; i < localVariableCount; ++i)
        crank_talk_ParseTreeAnalyzeAndBuild(context, node->localVariables->elements[i], buildContextValue);
    
    // Build the expressions
    gcFrame.result = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
    size_t expressionCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->expressions);
    crank_talk_oop_t *expressions = node->expressions->elements;
    for(size_t i = 0; i< expressionCount; ++i)
        gcFrame.result = crank_talk_ParseTreeAnalyzeAndBuild(context, expressions[i], buildContextValue);
    
    buildContext->builder->lexicalEnvironment = gcFrame.oldLexicalEnvironment;
    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return (crank_talk_oop_t)gcFrame.result;
}


static crank_talk_oop_t
crank_talk_ParseTreeSequenceNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    assert(argumentCount >= 2);
    crank_talk_ParseTreeSequenceNode_t *node = (crank_talk_ParseTreeSequenceNode_t*)arguments[0];
    crank_talk_oop_t buildContext = arguments[1];

    if(crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->localVariables) != 0)
        return crank_talk_ParseTreeSequenceNodeWithLocals_analyzeAndBuildWith(context, closure, argumentCount, arguments);
    
    crank_talk_HIRValue_t *result = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
    size_t expressionCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->expressions);
    crank_talk_oop_t *expressions = node->expressions->elements;
    for(size_t i = 0; i< expressionCount; ++i)
        result = crank_talk_ParseTreeAnalyzeAndBuild(context, expressions[i], buildContext);
    return (crank_talk_oop_t)result;
}

static crank_talk_oop_t
crank_talk_ParseTreeVariableDefinitionNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    assert(argumentCount >= 2);
    crank_talk_ParseTreeVariableDefinitionNode_t *node = (crank_talk_ParseTreeVariableDefinitionNode_t*)arguments[0];
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)arguments[1];

    crank_talk_oop_t alloca = crank_talk_HIRBuilder_alloca(context, buildContext->builder->allocaBuilder, (crank_talk_oop_t)node->super.sourcePosition);
    crank_talk_LexicalEnvironment_setNewHIRSymbolBinding(context, buildContext->builder->lexicalEnvironment, (crank_talk_oop_t)node->name, alloca, (crank_talk_oop_t)node->super.sourcePosition);

    return crank_talk_HIRConstantLiteralValue_create(context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
}

static crank_talk_oop_t
crank_talk_ParseTreeCullBlockNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    assert(argumentCount >= 2);
    crank_talk_ParseTreeCullBlockNode_t *node = (crank_talk_ParseTreeCullBlockNode_t*)arguments[0];
    crank_talk_oop_t buildContextValue = arguments[1];

    //crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;

    // Make sure we have an inlinable block
    if(!crank_talk_oop_isKindOf(context, (crank_talk_oop_t)node->blockExpression, context->roots.ParseTreeBlockNodeClass))
        crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)node->blockExpression->sourcePosition, "Expected an inlinable block expression.");

    crank_talk_ParseTreeBlockNode_t *blockNode = (crank_talk_ParseTreeBlockNode_t*)node->blockExpression;

    // TODO: Make an environment and set the block arguments.

    return (crank_talk_oop_t)crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)blockNode->body, buildContextValue);
}

static crank_talk_oop_t
crank_talk_ParseTreeIfSelectionNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    assert(argumentCount >= 2);
    crank_talk_ParseTreeIfSelectionNode_t *node = (crank_talk_ParseTreeIfSelectionNode_t*)arguments[0];
    crank_talk_oop_t buildContextValue = arguments[1];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;
    crank_talk_HIRBuilder_t *builder = buildContext->builder;

    struct {
        crank_talk_HIRValue_t *nilLiteral;

        crank_talk_HIRValue_t *condition;

        crank_talk_HIRBasicBlock_t *trueDestination;
        crank_talk_HIRBuilder_t *trueBuilder;
        crank_talk_oop_t trueBuildPass;
        crank_talk_HIRValue_t *trueResult;
        
        crank_talk_HIRBasicBlock_t *falseDestination;
        crank_talk_HIRBuilder_t *falseBuilder;
        crank_talk_oop_t falseBuildPass;
        crank_talk_HIRValue_t *falseResult;

        crank_talk_HIRBasicBlock_t *mergeDestination;
        crank_talk_HIRPhiInstruction_t *mergePhi;
    } gcFrame = {0};

    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.nilLiteral = (crank_talk_HIRValue_t*)crank_talk_HIRConstantLiteralValue_create(context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);

    gcFrame.trueDestination = crank_talk_HIRBasicBlock_createNamed(context, "ifTrue");
    gcFrame.falseDestination = crank_talk_HIRBasicBlock_createNamed(context, "ifFalse");
    gcFrame.mergeDestination = crank_talk_HIRBasicBlock_createNamed(context, "ifMerge");

    gcFrame.condition = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->condition, buildContextValue);
    crank_talk_HIRBuilder_conditionalBranch(context, builder, gcFrame.condition, gcFrame.trueDestination, gcFrame.falseDestination, (crank_talk_oop_t)node->super.sourcePosition);

    // True destination
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.trueDestination);
    gcFrame.trueBuilder = crank_talk_HIRBuilder_copyWithBasicBlock(context, builder, gcFrame.trueDestination);
    gcFrame.trueBuildPass = crank_talk_createAnalysisAndBuildContext(context, gcFrame.trueBuilder);
    gcFrame.trueResult = gcFrame.nilLiteral;
    if(node->trueExpression)
        gcFrame.trueResult = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->trueExpression, gcFrame.trueBuildPass);

    // False destination
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.falseDestination);
    gcFrame.falseBuilder = crank_talk_HIRBuilder_copyWithBasicBlock(context, builder, gcFrame.falseDestination);
    gcFrame.falseBuildPass = crank_talk_createAnalysisAndBuildContext(context, gcFrame.falseBuilder);
    gcFrame.falseResult = gcFrame.nilLiteral;
    if(node->falseExpression)
        gcFrame.falseResult = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->falseExpression, gcFrame.falseBuildPass);

    // Merge
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.mergeDestination);
    builder->basicBlock = gcFrame.mergeDestination;

    gcFrame.mergePhi = crank_talk_HIRBuilder_phi(context, builder, (crank_talk_oop_t)node->super.sourcePosition);

    if(!crank_talk_HIRBuilder_isLastTerminator(context, gcFrame.trueBuilder))
    {
        crank_talk_HIRBuilder_phiSource(context, gcFrame.trueBuilder, gcFrame.mergePhi, gcFrame.trueResult, (crank_talk_oop_t)node->super.sourcePosition);
        crank_talk_HIRBuilder_branch(context, gcFrame.trueBuilder, gcFrame.mergeDestination, (crank_talk_oop_t)node->super.sourcePosition);
    }

    if(!crank_talk_HIRBuilder_isLastTerminator(context, gcFrame.falseBuilder))
    {
        crank_talk_HIRBuilder_phiSource(context, gcFrame.falseBuilder, gcFrame.mergePhi, gcFrame.falseResult, (crank_talk_oop_t)node->super.sourcePosition);
        crank_talk_HIRBuilder_branch(context, gcFrame.falseBuilder, gcFrame.mergeDestination, (crank_talk_oop_t)node->super.sourcePosition);
    }

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return (crank_talk_oop_t)gcFrame.mergePhi;
}

void
crank_talk_registerAnalysisAndBuildPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LiteralVariableClass, "analyzeAndBuildAssignment:withValue:with:", 4, crank_talk_LiteralVariable_analyzeAndBuildAssignmentWithWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRVariableClass, "analyzeAndBuildAssignment:withValue:with:", 4, crank_talk_HIRVariable_analyzeAndBuildAssignmentWithWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRAllocaInstructionClass, "analyzeAndBuildAssignment:withValue:with:", 4, crank_talk_HIRAlloca_analyzeAndBuildAssignmentWithWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LiteralVariableClass, "analyzeAndBuildIdentifierReference:with:", 3, crank_talk_LiteralVariable_analyzeAndBuildIdentifierReferenceWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRVariableClass, "analyzeAndBuildIdentifierReference:with:", 3, crank_talk_HIRVariable_analyzeAndBuildIdentifierReferenceWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRValueClass, "analyzeAndBuildIdentifierReference:with:", 3, crank_talk_HIRValue_analyzeAndBuildIdentifierReferenceWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRAllocaInstructionClass, "analyzeAndBuildIdentifierReference:with:", 3, crank_talk_HIRAllocaInstruction_analyzeAndBuildIdentifierReferenceWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeAssignmentNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeAssignmentNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeBlockNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeBlockNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeIdentifierReferenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeIdentifierReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeLiteralNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeLiteralNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeNilReferenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeNilReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeFalseReferenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeFalseReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeTrueReferenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeTrueReferenceNode_analyzeAndBuildWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageCascadeNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeMessageCascadeNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageSendNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeMessageSendNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSequenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeSequenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeVariableDefinitionNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeVariableDefinitionNode_analyzeAndBuildWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeCullBlockNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeCullBlockNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeIfSelectionNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeIfSelectionNode_analyzeAndBuildWith);
}

