#include "crank-talk/analysis-build.h"
#include "crank-talk/context.h"
#include "crank-talk/hir.h"
#include "crank-talk/message.h"
#include "crank-talk/parse-tree.h"

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
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSequenceNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ParseTreeSequenceNode_analyzeAndBuildWith);
}

