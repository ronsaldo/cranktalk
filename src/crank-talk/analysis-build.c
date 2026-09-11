#include "crank-talk/analysis-build.h"
#include "crank-talk/context.h"
#include "crank-talk/environment.h"
#include "crank-talk/errors.h"
#include "crank-talk/hir.h"
#include "crank-talk/message.h"
#include "crank-talk/parse-tree.h"
#include "crank-talk/string.h"
#include <stdlib.h>

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
crank_talk_ParseTreeSequenceNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeSequenceNode_t *node = (crank_talk_ParseTreeSequenceNode_t*)arguments[0];
    crank_talk_oop_t buildContext = arguments[1];
    
    crank_talk_HIRValue_t *result = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
    size_t expressionCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->expressions);
    crank_talk_oop_t *expressions = node->expressions->elements;
    for(size_t i = 0; i< expressionCount; ++i)
        result = crank_talk_ParseTreeAnalyzeAndBuild(context, expressions[i], buildContext);
    return (crank_talk_oop_t)result;
}

void
crank_talk_registerAnalysisAndBuildPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRVariableClass, "analyzeAndBuildIdentifierReference:with:", 3, crank_talk_HIRVariable_analyzeAndBuildIdentifierReferenceWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRValueClass, "analyzeAndBuildIdentifierReference:with:", 3, crank_talk_HIRValue_analyzeAndBuildIdentifierReferenceWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeIdentifierReferenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeIdentifierReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeLiteralNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeLiteralNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeNilReferenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeNilReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeFalseReferenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeFalseReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeTrueReferenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeTrueReferenceNode_analyzeAndBuildWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSequenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeSequenceNode_analyzeAndBuildWith);
}

