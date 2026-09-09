#include "crank-talk/analysis-evaluation.h"
#include "crank-talk/context.h"
#include "crank-talk/environment.h"
#include "crank-talk/message.h"
#include "crank-talk/parse-tree.h"
#include "crank-talk/stack-frame.h"
#include "crank-talk/string.h"

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
crank_talk_ParseTreeSequenceNode_analyzeAndEvaluateWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t evaluationContext = arguments[1];

    crank_talk_ParseTreeSequenceNode_t *node = (crank_talk_ParseTreeSequenceNode_t*)self;
   
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

void
crank_talk_registerAnalysisAndEvaluationPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeLiteralNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeLiteralNode_analyzeAndEvaluateWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSequenceNodeClass, "analyzeAndEvaluateWith:", 2, crank_talk_ParseTreeSequenceNode_analyzeAndEvaluateWith);
}