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

static void
crank_talk_LiteralVariable_analyzeAndBuildAssignmentWithWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 4) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_ParseTreeAssignmentNode_t *assignmentNode =(crank_talk_ParseTreeAssignmentNode_t*)primContext->arguments[1];
    crank_talk_HIRValue_t *assignedValue = (crank_talk_HIRValue_t*)primContext->arguments[2];
    crank_talk_oop_t buildContextValue = primContext->arguments[3];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_HIRValue_t *literalVariableValue = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(primContext->context, self, (crank_talk_oop_t)assignmentNode->super.sourcePosition);
    crank_talk_HIRBuilder_variableStore(primContext->context, buildContext->builder, literalVariableValue, assignedValue, (crank_talk_oop_t)assignmentNode->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)assignedValue);
}

static void
crank_talk_HIRVariable_analyzeAndBuildAssignmentWithWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 4) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t assignmentNode = primContext->arguments[1];
    crank_talk_oop_t assignedValue = primContext->arguments[2];
    crank_talk_oop_t buildContextValue = primContext->arguments[3];

    crank_talk_HIRVariable_t *hirVariable = (crank_talk_HIRVariable_t *)self;
    crank_talk_oop_t result = crank_talk_send3(primContext->context, primContext->context->roots.analyzeAndBuildAssignmentWithValueWithSymbol, hirVariable->value, assignmentNode, assignedValue, buildContextValue);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_HIRAlloca_analyzeAndBuildAssignmentWithWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 4) return crank_talk_Primitive_fail(primContext);

    crank_talk_HIRValue_t* self = (crank_talk_HIRValue_t*)primContext->arguments[0];
    crank_talk_ParseTreeAssignmentNode_t *assignmentNode =(crank_talk_ParseTreeAssignmentNode_t*)primContext->arguments[1];
    crank_talk_HIRValue_t *assignedValue = (crank_talk_HIRValue_t*)primContext->arguments[2];
    crank_talk_oop_t buildContextValue = primContext->arguments[3];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_HIRBuilder_variableStore(primContext->context, buildContext->builder, self, assignedValue, (crank_talk_oop_t)assignmentNode->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)assignedValue);
}

static void
crank_talk_HIRCapture_analyzeAndBuildAssignmentWithWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 4) return crank_talk_Primitive_fail(primContext);

    crank_talk_HIRValue_t* self = (crank_talk_HIRValue_t*)primContext->arguments[0];
    crank_talk_ParseTreeAssignmentNode_t *assignmentNode = (crank_talk_ParseTreeAssignmentNode_t*)primContext->arguments[1];
    crank_talk_HIRValue_t *assignedValue = (crank_talk_HIRValue_t*)primContext->arguments[2];
    crank_talk_oop_t buildContextValue = primContext->arguments[3];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_HIRCapture_t *capture = (crank_talk_HIRCapture_t *)self;
    if(!crank_talk_oop_asBoolean(primContext->context, capture->isVariable))
        crank_talk_errorPrintfAtSourcePosition(primContext->context, self->sourcePosition, "Captured value is not a variable.");

    crank_talk_HIRBuilder_variableStore(primContext->context, buildContext->builder, self, assignedValue, (crank_talk_oop_t)assignmentNode->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)assignedValue);
}

static void
crank_talk_Slot_analyzeAndBuildAssignmentWithWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 4) return crank_talk_Primitive_fail(primContext);

    crank_talk_Slot_t* self = (crank_talk_Slot_t*)primContext->arguments[0];
    crank_talk_ParseTreeAssignmentNode_t *assignmentNode =(crank_talk_ParseTreeAssignmentNode_t*)primContext->arguments[1];
    crank_talk_HIRValue_t *assignedValue = (crank_talk_HIRValue_t*)primContext->arguments[2];
    crank_talk_oop_t buildContextValue = primContext->arguments[3];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    size_t slotIndex = crank_talk_oop_decodeSmallInteger(self->index);

    crank_talk_oop_t receiverVariable = crank_talk_Environment_lookupReceiverRecursively(primContext->context, buildContext->builder->lexicalEnvironment);
    assert(crank_talk_oop_isKindOf(primContext->context, receiverVariable, primContext->context->roots.HIRVariableClass));
        
    crank_talk_oop_t receiverValue = ((crank_talk_HIRVariable_t*)receiverVariable)->value;
    assert(crank_talk_oop_isKindOf(primContext->context, receiverValue, primContext->context->roots.HIRValueClass));

    crank_talk_HIRBuilder_slotStore(primContext->context, buildContext->builder, (crank_talk_HIRValue_t*)receiverValue, slotIndex, assignedValue, (crank_talk_oop_t)assignmentNode->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)assignedValue);
}

static void
crank_talk_ParseTreeAssignmentNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];

    crank_talk_ParseTreeAssignmentNode_t *node = (crank_talk_ParseTreeAssignmentNode_t*)self;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_HIRValue_t *assignedValue = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->value, buildContextValue);

    if(!crank_talk_oop_isKindOf(primContext->context, (crank_talk_oop_t)node->store, (crank_talk_oop_t)primContext->context->roots.ParseTreeIdentifierReferenceNodeClass))
        crank_talk_errorAtSourcePosition(primContext->context, (crank_talk_oop_t)node->store->sourcePosition, "Expected an identifier with the variable name to assign.");

    crank_talk_ParseTreeIdentifierReferenceNode_t *storeIdentifer = (crank_talk_ParseTreeIdentifierReferenceNode_t*)node->store;
    crank_talk_oop_t foundBinding = crank_talk_Environment_lookupSymbolRecursively(primContext->context, buildContext->builder->lexicalEnvironment, (crank_talk_oop_t)storeIdentifer->symbol);
    if(crank_talk_oop_isNil(foundBinding))
        crank_talk_errorPrintfAtSourcePosition(primContext->context, (crank_talk_oop_t)node->super.sourcePosition, "Binding not found for symbol #" CRANK_TALK_STRING_FORMAT, CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)storeIdentifer->symbol));

    crank_talk_oop_t result = crank_talk_send3(primContext->context, primContext->context->roots.analyzeAndBuildAssignmentWithValueWithSymbol, foundBinding, self, (crank_talk_oop_t)assignedValue, buildContextValue);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeBlockNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];

    crank_talk_ParseTreeBlockNode_t *node = (crank_talk_ParseTreeBlockNode_t*)self;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

        struct {
        crank_talk_oop_t compiledFunctionValue;
        crank_talk_oop_t blockClosureValue;
    } gcFrame = {};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Compile the function
    gcFrame.compiledFunctionValue = CrankAllocateValuesObjectWithClassName(primContext->context, CompiledFunction);
    crank_talk_CompiledFunction_t *compiledFunction = (crank_talk_CompiledFunction_t*)gcFrame.compiledFunctionValue;
    compiledFunction->argumentCount = crank_talk_oop_encodeSmallInteger(crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->arguments));
    compiledFunction->isBlock = crank_talk_oop_fromBoolean(primContext->context, true);
    compiledFunction->isInnerBlock = crank_talk_oop_fromBoolean(primContext->context, true);
    compiledFunction->definitionEnvironment = buildContext->builder->lexicalEnvironment;
    compiledFunction->definitionArguments = (crank_talk_Array_t*)node->arguments;
    compiledFunction->definitionBody = (crank_talk_oop_t)node->body;
    crank_talk_CompiledFunction_ensureAnalysis(primContext->context, compiledFunction);

    crank_talk_HIRFunction_t *hirFunction = (crank_talk_HIRFunction_t *)compiledFunction->hirFunction;
    size_t captureCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)hirFunction->captures);
    if(captureCount == 0 && false)
    {
        // FIXME: create a captureless block closure if it does not have a non-local return
        // Create the captureless block closure
        gcFrame.blockClosureValue = CrankAllocateValuesObjectWithClassName(primContext->context, BlockClosure);
        crank_talk_BlockClosure_t *blockClosure = (crank_talk_BlockClosure_t *)gcFrame.blockClosureValue;
        blockClosure->captureVector = (crank_talk_Array_t*)crank_talk_Array_create(primContext->context, 0);
        blockClosure->function = compiledFunction;

        crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, gcFrame.blockClosureValue, (crank_talk_oop_t)node->super.sourcePosition);
        CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
        return crank_talk_Primitive_return(primContext, result);
    }
    else
    {
        crank_talk_Array_t *capturedValues = (crank_talk_Array_t*)crank_talk_Array_create(primContext->context, captureCount);
        for(size_t i = 0; i < captureCount; ++i)
        {
            crank_talk_HIRCapture_t *capture = (crank_talk_HIRCapture_t*)hirFunction->captures->elements[i];
            capturedValues->elements[i] = (crank_talk_oop_t)capture->capturedValue;
        }

        crank_talk_oop_t result = crank_talk_HIRBuilder_makeClosure(primContext->context, buildContext->builder, compiledFunction, capturedValues, (crank_talk_oop_t)node->super.sourcePosition);
        CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
        return crank_talk_Primitive_return(primContext, result);
    }
}

static void
crank_talk_LiteralVariable_analyzeAndBuildIdentifierReferenceWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t referenceNode = primContext->arguments[1];
    crank_talk_oop_t buildContextValue = primContext->arguments[2];

    crank_talk_ParseTreeIdentifierReferenceNode_t *node = (crank_talk_ParseTreeIdentifierReferenceNode_t*)referenceNode;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_HIRValue_t *variableValue = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(primContext->context, self, (crank_talk_oop_t)node->super.sourcePosition);
    crank_talk_oop_t result = crank_talk_HIRBuilder_variableLoad(primContext->context, buildContext->builder, variableValue, (crank_talk_oop_t)node->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_HIRVariable_analyzeAndBuildIdentifierReferenceWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t referenceNode = primContext->arguments[1];
    crank_talk_oop_t buildContextValue = primContext->arguments[2];

    crank_talk_HIRVariable_t *hirVariable = (crank_talk_HIRVariable_t *)self;
    crank_talk_oop_t result = crank_talk_send2(primContext->context, primContext->context->roots.analyzeAndBuildIdentifierReferenceWithSymbol, hirVariable->value, referenceNode, buildContextValue);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_HIRValue_analyzeAndBuildIdentifierReferenceWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    //crank_talk_oop_t referenceNode = arguments[1];
    //crank_talk_oop_t buildContextValue = arguments[2];

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_HIRAllocaInstruction_analyzeAndBuildIdentifierReferenceWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    crank_talk_HIRValue_t *self = (crank_talk_HIRValue_t*)primContext->arguments[0];
    crank_talk_oop_t referenceNode = primContext->arguments[1];
    crank_talk_oop_t buildContextValue = primContext->arguments[2];
    
    crank_talk_ParseTreeIdentifierReferenceNode_t *node = (crank_talk_ParseTreeIdentifierReferenceNode_t*)referenceNode;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_oop_t result = crank_talk_HIRBuilder_variableLoad(primContext->context, buildContext->builder, self, (crank_talk_oop_t)node->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_HIRCaptureInstruction_analyzeAndBuildIdentifierReferenceWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    crank_talk_HIRCapture_t *self = (crank_talk_HIRCapture_t*)primContext->arguments[0];
    crank_talk_oop_t referenceNode = primContext->arguments[1];
    crank_talk_oop_t buildContextValue = primContext->arguments[2];
    
    crank_talk_ParseTreeIdentifierReferenceNode_t *node = (crank_talk_ParseTreeIdentifierReferenceNode_t*)referenceNode;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    if(crank_talk_oop_asBoolean(primContext->context, self->isVariable))
    {
        crank_talk_oop_t result = crank_talk_HIRBuilder_variableLoad(primContext->context, buildContext->builder, &self->super.super, (crank_talk_oop_t)node->super.sourcePosition);
        return crank_talk_Primitive_return(primContext, result);
    }
    else
    {
        return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)self);
    }
}

static void
crank_talk_Slot_analyzeAndBuildIdentifierReferenceWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    crank_talk_Slot_t *self = (crank_talk_Slot_t*)primContext->arguments[0];
    crank_talk_oop_t referenceNode = primContext->arguments[1];
    crank_talk_oop_t buildContextValue = primContext->arguments[2];
    
    crank_talk_ParseTreeIdentifierReferenceNode_t *node = (crank_talk_ParseTreeIdentifierReferenceNode_t*)referenceNode;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    //return crank_talk_HIRBuilder_variableLoad(context, buildContext->builder, self, (crank_talk_oop_t)node->super.sourcePosition);
    size_t slotIndex = crank_talk_oop_decodeSmallInteger(self->index);

    crank_talk_oop_t receiverVariable = crank_talk_Environment_lookupReceiverRecursively(primContext->context, buildContext->builder->lexicalEnvironment);
    crank_talk_oop_t receiverValue = crank_talk_send2(primContext->context, primContext->context->roots.analyzeAndBuildIdentifierReferenceWithSymbol, receiverVariable, referenceNode, buildContextValue);
    assert(crank_talk_oop_isKindOf(primContext->context, receiverValue, primContext->context->roots.HIRValueClass));

    crank_talk_oop_t result = crank_talk_HIRBuilder_slotLoad(primContext->context, buildContext->builder, (crank_talk_HIRValue_t*)receiverValue, slotIndex, (crank_talk_oop_t)node->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeIdentifierReferenceNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];

    crank_talk_ParseTreeIdentifierReferenceNode_t *node = (crank_talk_ParseTreeIdentifierReferenceNode_t*)self;
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_oop_t foundBinding = crank_talk_Environment_lookupSymbolRecursively(primContext->context, buildContext->builder->lexicalEnvironment, (crank_talk_oop_t)node->symbol);
    if(crank_talk_oop_isNil(foundBinding))
        crank_talk_errorPrintfAtSourcePosition(primContext->context, (crank_talk_oop_t)node->super.sourcePosition, "Binding not found for symbol #" CRANK_TALK_STRING_FORMAT, CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)node->symbol));
    
    crank_talk_oop_t result = crank_talk_send2(primContext->context, primContext->context->roots.analyzeAndBuildIdentifierReferenceWithSymbol, foundBinding, self, buildContextValue);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeLiteralNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeLiteralNode_t *node = (crank_talk_ParseTreeLiteralNode_t*)primContext->arguments[0];

    // FIXME: Use a message here-
    if(crank_talk_oop_isKindOf(primContext->context, node->value, primContext->context->roots.HIRValueClass))
        return crank_talk_Primitive_return(primContext, node->value);
    
    crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, node->value, (crank_talk_oop_t)node->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeNilReferenceNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeNode_t *node = (crank_talk_ParseTreeNode_t*)primContext->arguments[0];
    crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, crank_talk_oop_nil(), (crank_talk_oop_t)node->sourcePosition);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeFalseReferenceNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeNode_t *node = (crank_talk_ParseTreeNode_t*)primContext->arguments[0];
    crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, primContext->context->roots.falseValue, (crank_talk_oop_t)node->sourcePosition);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeTrueReferenceNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeNode_t *node = (crank_talk_ParseTreeNode_t*)primContext->arguments[0];
    crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, primContext->context->roots.trueValue, (crank_talk_oop_t)node->sourcePosition);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeSelfReferenceNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeNode_t *node = (crank_talk_ParseTreeNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_oop_t foundBinding = crank_talk_Environment_lookupReceiverRecursively(primContext->context, buildContext->builder->lexicalEnvironment);
    if(!foundBinding)
    {
        crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, crank_talk_oop_nil(), (crank_talk_oop_t)node->sourcePosition);
        return crank_talk_Primitive_return(primContext, result);
    }
        

    assert(crank_talk_oop_isKindOf(primContext->context, foundBinding, primContext->context->roots.HIRVariableClass));
    return crank_talk_Primitive_return(primContext, ((crank_talk_HIRVariable_t*)foundBinding)->value);
}

static void
crank_talk_ParseTreeSuperReferenceNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeNode_t *node = (crank_talk_ParseTreeNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_oop_t foundBinding = crank_talk_Environment_lookupReceiverRecursively(primContext->context, buildContext->builder->lexicalEnvironment);
    if(!foundBinding)
    {
        crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, crank_talk_oop_nil(), (crank_talk_oop_t)node->sourcePosition);
        return crank_talk_Primitive_return(primContext, result);
    }
        
    assert(crank_talk_oop_isKindOf(primContext->context, foundBinding, primContext->context->roots.HIRVariableClass));
    return crank_talk_Primitive_return(primContext, ((crank_talk_HIRVariable_t*)foundBinding)->value);
}

static void
crank_talk_ParseTreeThisContextReferenceNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeNode_t *node = (crank_talk_ParseTreeNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t*)buildContextValue;

    crank_talk_oop_t result = crank_talk_HIRBuilder_thisContext(primContext->context, buildContext->builder, (crank_talk_oop_t)node->sourcePosition);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeMessageCascadeNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeMessageCascadeNode_t *node = (crank_talk_ParseTreeMessageCascadeNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];

    struct {
        crank_talk_HIRValue_t *receiver;
        crank_talk_ParseTreeLiteralNode_t *receiverNode;
        crank_talk_HIRValue_t *result;
    } gcFrame = {0};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Evaluate the receiver and make the 
    gcFrame.receiver = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->receiver, buildContextValue);
    gcFrame.receiverNode = (crank_talk_ParseTreeLiteralNode_t*)CrankAllocateValuesObjectWithClassName(primContext->context, ParseTreeLiteralNode);
    gcFrame.receiverNode->super.sourcePosition = node->receiver->sourcePosition;
    gcFrame.receiverNode->value = (crank_talk_oop_t)gcFrame.receiver;

    //crank_talk_ParseTreeLiteralNode_t *node = (crank_talk_ParseTreeLiteralNode_t*)arguments[0];
    gcFrame.result = gcFrame.receiver;

    size_t cascadedMessageCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->cascadedMessages);
    for(size_t i = 0; i < cascadedMessageCount; ++i)
    {
        crank_talk_oop_t cascadedMessageValue = node->cascadedMessages->elements[i];
        crank_talk_ParseTreeCascadedMessageNode_t *cascadedMessage = (crank_talk_ParseTreeCascadedMessageNode_t*)cascadedMessageValue;

        crank_talk_oop_t expandedCascadedMessage = crank_talk_ParseTreeCascadedMessageNode_asMessageSendWithReceiver(primContext->context, cascadedMessage, (crank_talk_oop_t)gcFrame.receiverNode);
        gcFrame.result = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, expandedCascadedMessage, buildContextValue);
    }

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)gcFrame.result);
}

static void
crank_talk_ParseTreeMessageSendNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeMessageSendNode_t *node = (crank_talk_ParseTreeMessageSendNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;

    struct {
        crank_talk_HIRValue_t *selector;
        crank_talk_oop_t selectorValue;
        crank_talk_HIRValue_t *receiver;
        crank_talk_Array_t *argumentsWithReceiver;
    } gcFrame = {0};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Fetch the selector value.
    gcFrame.selector = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->selector, buildContextValue);
    assert(crank_talk_oop_isKindOf(primContext->context, (crank_talk_oop_t)gcFrame.selector, primContext->context->roots.HIRConstantLiteralValueClass));
    gcFrame.selectorValue = ((crank_talk_HIRConstantLiteralValue_t*)gcFrame.selector)->value;

    // Is this a macro message send?
    if(crank_talk_IdentityDictionary_includesKey(primContext->context->roots.messageSendMacroDictionary, gcFrame.selectorValue))
    {
        crank_talk_oop_t messageMacro = crank_talk_IdentityDictionary_at(primContext->context, primContext->context->roots.messageSendMacroDictionary, gcFrame.selectorValue);
        crank_talk_oop_t expandedMacro = crank_talk_apply1(primContext->context, messageMacro, primContext->arguments[0]);
        CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
        crank_talk_oop_t result = (crank_talk_oop_t)crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, expandedMacro, buildContextValue);
        return crank_talk_Primitive_return(primContext, result);
    }

    bool isSuperSend = crank_talk_isParseTreeSuperReferenceNode(primContext->context, (crank_talk_oop_t)node->receiver);

    gcFrame.receiver = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->receiver, buildContextValue);
    size_t messageArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->arguments);
    gcFrame.argumentsWithReceiver = (crank_talk_Array_t*)crank_talk_Array_create(primContext->context, 1 + messageArgumentCount);
    gcFrame.argumentsWithReceiver->elements[0] = (crank_talk_oop_t)gcFrame.receiver;
    for(size_t i = 0; i < messageArgumentCount; ++i)
        gcFrame.argumentsWithReceiver->elements[i + 1] = (crank_talk_oop_t)crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, node->arguments->elements[i], buildContextValue);

    if(isSuperSend)
    {
        crank_talk_oop_t receiverClasss = crank_talk_Environment_lookupReceiverClassRecursively(primContext->context, buildContext->builder->lexicalEnvironment);
        crank_talk_HIRValue_t *receiverClassValue = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(primContext->context, receiverClasss, (crank_talk_oop_t)node->super.sourcePosition);
        if(!receiverClasss)
            crank_talk_error(primContext->context, "super is not properly defined in this context.");

        crank_talk_oop_t result = crank_talk_HIRBuilder_superSend(primContext->context, buildContext->builder, receiverClassValue, gcFrame.selector, gcFrame.argumentsWithReceiver, (crank_talk_oop_t)node->super.sourcePosition);
        CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);

        return crank_talk_Primitive_return(primContext, result);
    }
    else
    {
        crank_talk_oop_t result = crank_talk_HIRBuilder_send(primContext->context, buildContext->builder, gcFrame.selector, gcFrame.argumentsWithReceiver, (crank_talk_oop_t)node->super.sourcePosition);
        CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
        return crank_talk_Primitive_return(primContext, result);
    }
}

static void
crank_talk_ParseTreeReturnNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeReturnNode_t *node = (crank_talk_ParseTreeReturnNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];
    
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;

    crank_talk_oop_t isInnerBlock = crank_talk_Environment_lookupIsInnerBlockRecursively(primContext->context, buildContext->builder->lexicalEnvironment);

    crank_talk_HIRValue_t *resultValue = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->expression, buildContextValue);
    if(isInnerBlock == primContext->context->roots.trueValue)
        crank_talk_HIRBuilder_nonLocalReturn(primContext->context, buildContext->builder, resultValue, (crank_talk_oop_t)node->super.sourcePosition);
    else
        crank_talk_HIRBuilder_return(primContext->context, buildContext->builder, resultValue, (crank_talk_oop_t)node->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)resultValue);
}

static void
crank_talk_ParseTreeSequenceNodeWithLocals_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeSequenceNode_t *node = (crank_talk_ParseTreeSequenceNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;

    struct {
        crank_talk_oop_t oldLexicalEnvironment;
        crank_talk_HIRValue_t *result;
    } gcFrame = {0};
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.oldLexicalEnvironment = buildContext->builder->lexicalEnvironment;
    buildContext->builder->lexicalEnvironment = crank_talk_LexicalEnvironment_create(primContext->context, buildContext->builder->lexicalEnvironment);

    // Build the local variables
    size_t localVariableCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->localVariables);
    for(size_t i = 0; i < localVariableCount; ++i)
        crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, node->localVariables->elements[i], buildContextValue);
    
    // Build the expressions
    gcFrame.result = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(primContext->context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
    size_t expressionCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->expressions);
    crank_talk_oop_t *expressions = node->expressions->elements;
    for(size_t i = 0; i< expressionCount; ++i)
        gcFrame.result = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, expressions[i], buildContextValue);
    
    buildContext->builder->lexicalEnvironment = gcFrame.oldLexicalEnvironment;
    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)gcFrame.result);
}

static void
crank_talk_ParseTreeSequenceNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeSequenceNode_t *node = (crank_talk_ParseTreeSequenceNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContext = primContext->arguments[1];

    if(crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->localVariables) != 0)
        return crank_talk_ParseTreeSequenceNodeWithLocals_analyzeAndBuildWith(primContext);
    
    crank_talk_HIRValue_t *result = (crank_talk_HIRValue_t *)crank_talk_HIRConstantLiteralValue_create(primContext->context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
    size_t expressionCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)node->expressions);
    crank_talk_oop_t *expressions = node->expressions->elements;
    for(size_t i = 0; i< expressionCount; ++i)
        result = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, expressions[i], buildContext);
    
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)result);
}

static void
crank_talk_ParseTreeVariableDefinitionNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeVariableDefinitionNode_t *node = (crank_talk_ParseTreeVariableDefinitionNode_t*)primContext->arguments[0];
    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)primContext->arguments[1];

    crank_talk_oop_t alloca = crank_talk_HIRBuilder_alloca(primContext->context, buildContext->builder->allocaBuilder, (crank_talk_oop_t)node->super.sourcePosition);
    crank_talk_LexicalEnvironment_setNewHIRSymbolBinding(primContext->context, buildContext->builder->lexicalEnvironment, (crank_talk_oop_t)node->name, alloca, (crank_talk_oop_t)node->super.sourcePosition);

    crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeCullBlockNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeCullBlockNode_t *node = (crank_talk_ParseTreeCullBlockNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];

    //crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;

    // Make sure we have an inlinable block
    if(!crank_talk_oop_isKindOf(primContext->context, (crank_talk_oop_t)node->blockExpression, primContext->context->roots.ParseTreeBlockNodeClass))
        crank_talk_errorAtSourcePosition(primContext->context, (crank_talk_oop_t)node->blockExpression->sourcePosition, "Expected an inlinable block expression.");

    crank_talk_ParseTreeBlockNode_t *blockNode = (crank_talk_ParseTreeBlockNode_t*)node->blockExpression;

    // TODO: Make an environment and set the block arguments.

    crank_talk_oop_t result = (crank_talk_oop_t)crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)blockNode->body, buildContextValue);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeIfSelectionNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeIfSelectionNode_t *node = (crank_talk_ParseTreeIfSelectionNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];

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

    gcFrame.nilLiteral = (crank_talk_HIRValue_t*)crank_talk_HIRConstantLiteralValue_create(primContext->context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);

    gcFrame.trueDestination = crank_talk_HIRBasicBlock_createNamed(primContext->context, "ifTrue");
    gcFrame.falseDestination = crank_talk_HIRBasicBlock_createNamed(primContext->context, "ifFalse");
    gcFrame.mergeDestination = crank_talk_HIRBasicBlock_createNamed(primContext->context, "ifMerge");

    gcFrame.condition = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->condition, buildContextValue);
    crank_talk_HIRBuilder_conditionalBranch(primContext->context, builder, gcFrame.condition, gcFrame.trueDestination, gcFrame.falseDestination, (crank_talk_oop_t)node->super.sourcePosition);

    // True destination
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.trueDestination);
    gcFrame.trueBuilder = crank_talk_HIRBuilder_copyWithBasicBlock(primContext->context, builder, gcFrame.trueDestination);
    gcFrame.trueBuildPass = crank_talk_createAnalysisAndBuildContext(primContext->context, gcFrame.trueBuilder);
    gcFrame.trueResult = gcFrame.nilLiteral;
    if(node->trueExpression)
        gcFrame.trueResult = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->trueExpression, gcFrame.trueBuildPass);

    // False destination
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.falseDestination);
    gcFrame.falseBuilder = crank_talk_HIRBuilder_copyWithBasicBlock(primContext->context, builder, gcFrame.falseDestination);
    gcFrame.falseBuildPass = crank_talk_createAnalysisAndBuildContext(primContext->context, gcFrame.falseBuilder);
    gcFrame.falseResult = gcFrame.nilLiteral;
    if(node->falseExpression)
        gcFrame.falseResult = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->falseExpression, gcFrame.falseBuildPass);

    // Merge
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.mergeDestination);
    builder->basicBlock = gcFrame.mergeDestination;

    gcFrame.mergePhi = crank_talk_HIRBuilder_phi(primContext->context, builder, (crank_talk_oop_t)node->super.sourcePosition);

    if(!crank_talk_HIRBuilder_isLastTerminator(primContext->context, gcFrame.trueBuilder))
    {
        crank_talk_HIRBuilder_phiSource(primContext->context, gcFrame.trueBuilder, gcFrame.mergePhi, gcFrame.trueResult, (crank_talk_oop_t)node->super.sourcePosition);
        crank_talk_HIRBuilder_branch(primContext->context, gcFrame.trueBuilder, gcFrame.mergeDestination, (crank_talk_oop_t)node->super.sourcePosition);
    }

    if(!crank_talk_HIRBuilder_isLastTerminator(primContext->context, gcFrame.falseBuilder))
    {
        crank_talk_HIRBuilder_phiSource(primContext->context, gcFrame.falseBuilder, gcFrame.mergePhi, gcFrame.falseResult, (crank_talk_oop_t)node->super.sourcePosition);
        crank_talk_HIRBuilder_branch(primContext->context, gcFrame.falseBuilder, gcFrame.mergeDestination, (crank_talk_oop_t)node->super.sourcePosition);
    }

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)gcFrame.mergePhi);
}

static void
crank_talk_ParseTreeWhileNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeWhileNode_t *node = (crank_talk_ParseTreeWhileNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;
    crank_talk_HIRBuilder_t *builder = buildContext->builder;

    struct {
        crank_talk_HIRBasicBlock_t *loopHeader;
        crank_talk_HIRBasicBlock_t *loopBody;
        crank_talk_HIRBasicBlock_t *loopMerge;

    } gcFrame = {0};

    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.loopHeader = crank_talk_HIRBasicBlock_createNamed(primContext->context, "loopHeader");
    gcFrame.loopBody = crank_talk_HIRBasicBlock_createNamed(primContext->context, "loopBody");
    gcFrame.loopMerge = crank_talk_HIRBasicBlock_createNamed(primContext->context, "loopMerge");

    // Loop header
    crank_talk_HIRBuilder_branch(primContext->context, builder, gcFrame.loopHeader, (crank_talk_oop_t)node->super.sourcePosition);
    builder->basicBlock = gcFrame.loopHeader;
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.loopHeader);

    if(node->condition)
    {
        crank_talk_HIRValue_t *condition = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->condition, buildContextValue);
        crank_talk_HIRBuilder_conditionalBranch(primContext->context, builder, condition, gcFrame.loopBody, gcFrame.loopMerge, (crank_talk_oop_t)node->super.sourcePosition);
    }
    else
    {
        crank_talk_HIRBuilder_branch(primContext->context, builder, gcFrame.loopBody, (crank_talk_oop_t)node->super.sourcePosition);
    }

    // Loop body.
    builder->basicBlock = gcFrame.loopBody;
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.loopBody);

    if(node->bodyExpression)
        crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->bodyExpression, buildContextValue);

    if(!crank_talk_HIRBuilder_isLastTerminator(primContext->context, builder))
        crank_talk_HIRBuilder_branch(primContext->context, builder, gcFrame.loopHeader, (crank_talk_oop_t)node->super.sourcePosition);

    // Loop merge
    builder->basicBlock = gcFrame.loopMerge;
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.loopMerge);

    crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_ParseTreeDoWhileNode_analyzeAndBuildWith(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_ParseTreeDoWhileNode_t *node = (crank_talk_ParseTreeDoWhileNode_t*)primContext->arguments[0];
    crank_talk_oop_t buildContextValue = primContext->arguments[1];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;
    crank_talk_HIRBuilder_t *builder = buildContext->builder;

    struct {
        crank_talk_HIRBasicBlock_t *loopBody;
        crank_talk_HIRBasicBlock_t *loopMerge;

    } gcFrame = {0};

    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    gcFrame.loopBody = crank_talk_HIRBasicBlock_createNamed(primContext->context, "loopBody");
    gcFrame.loopMerge = crank_talk_HIRBasicBlock_createNamed(primContext->context, "loopMerge");

    // Loop body
    crank_talk_HIRBuilder_branch(primContext->context, builder, gcFrame.loopBody, (crank_talk_oop_t)node->super.sourcePosition);
    builder->basicBlock = gcFrame.loopBody;
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.loopBody);

    if(node->bodyExpression)
        crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->bodyExpression, buildContextValue);

    if(!crank_talk_HIRBuilder_isLastTerminator(primContext->context, builder))
    {
        if(node->condition)
        {
            crank_talk_HIRValue_t *condition = crank_talk_ParseTreeAnalyzeAndBuild(primContext->context, (crank_talk_oop_t)node->condition, buildContextValue);
            crank_talk_HIRBuilder_conditionalBranch(primContext->context, builder, condition, gcFrame.loopBody, gcFrame.loopMerge, (crank_talk_oop_t)node->super.sourcePosition);
        }
        else
        {
            crank_talk_HIRBuilder_branch(primContext->context, builder, gcFrame.loopBody, (crank_talk_oop_t)node->super.sourcePosition);
        }
    }

    // Loop merge
    builder->basicBlock = gcFrame.loopMerge;
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.loopMerge);

    crank_talk_oop_t result = crank_talk_HIRConstantLiteralValue_create(primContext->context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return crank_talk_Primitive_return(primContext, result);
}

static crank_talk_oop_t
crank_talk_ParseTree_cullInlineBlock(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *blockValueNode, crank_talk_oop_t buildContextValue, crank_talk_HIRValue_t *cullArgument)
{
    if(!crank_talk_isParseTreeBlockNode(context, (crank_talk_oop_t)blockValueNode))
        crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)blockValueNode->sourcePosition, "Expected an inlinable block.");

    crank_talk_ParseTreeBlockNode_t *blockNode = (crank_talk_ParseTreeBlockNode_t *)blockValueNode;

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;
    crank_talk_HIRBuilder_t *builder = buildContext->builder;

    size_t blockArgumentCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)blockNode->arguments);
    if(blockArgumentCount > 1)
        crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)blockNode->super.sourcePosition, "Expected a block with at most one argument.");

    struct {
        crank_talk_oop_t oldLexicalEnvironment;
        crank_talk_oop_t result;
    } gcFrame = {
        .oldLexicalEnvironment = builder->lexicalEnvironment,
        .result = crank_talk_oop_nil()
    };
    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    builder->lexicalEnvironment = crank_talk_LexicalEnvironment_create(context, builder->lexicalEnvironment);

    crank_talk_oop_t *blockArguments = blockNode->arguments->elements;
    if(blockArgumentCount == 1)
    {
        crank_talk_oop_t argumentDefinitionValue = blockArguments[0];

        if(!crank_talk_oop_isKindOf(context, argumentDefinitionValue, context->roots.ParseTreeArgumentDefinitionNodeClass))
            crank_talk_errorAtSourcePosition(context, (crank_talk_oop_t)((crank_talk_ParseTreeNode_t*)argumentDefinitionValue)->sourcePosition, "Expected an argument definition node.");

        crank_talk_ParseTreeArgumentDefinitionNode_t *argumentDefinition = (crank_talk_ParseTreeArgumentDefinitionNode_t*)argumentDefinitionValue;
        crank_talk_LexicalEnvironment_setHIRSymbolBinding(context, builder->lexicalEnvironment, (crank_talk_oop_t)argumentDefinition->name, (crank_talk_oop_t)cullArgument);
    }


    gcFrame.result = (crank_talk_oop_t)crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)blockNode->body, buildContextValue);

    builder->lexicalEnvironment = gcFrame.oldLexicalEnvironment;
    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);

    return gcFrame.result;

}

static crank_talk_oop_t
crank_talk_ToByDoNode_analyzeAndBuildWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeToByDoNode_t *node = (crank_talk_ParseTreeToByDoNode_t*)arguments[0];
    crank_talk_oop_t buildContextValue = arguments[1];

    crank_talk_AnalysisAndBuildContext_t *buildContext = (crank_talk_AnalysisAndBuildContext_t *)buildContextValue;
    crank_talk_HIRBuilder_t *builder = buildContext->builder;

    struct {
        crank_talk_HIRValue_t *currentNumberVariable;

        crank_talk_HIRValue_t *startValue;
        crank_talk_HIRValue_t *stepValue;
        crank_talk_HIRValue_t *stopValue;

        crank_talk_HIRBasicBlock_t *loopHeader;
        crank_talk_HIRBasicBlock_t *loopBody;
        crank_talk_HIRBasicBlock_t *loopMerge;
    } gcFrame = {0};

    CRANK_TALK_STACKFRAME_PUSH_GC_ROOTS(gcFrameRecord, gcFrame);

    // Fetch the parameters
    gcFrame.startValue = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->startExpression, buildContextValue);
    gcFrame.stopValue = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->stopExpression, buildContextValue);
    gcFrame.stepValue = (crank_talk_HIRValue_t*)crank_talk_HIRConstantLiteralValue_create(context, crank_talk_oop_encodeSmallInteger(1), (crank_talk_oop_t)node->super.sourcePosition);
    if(node->stepExpression)
        gcFrame.stepValue = crank_talk_ParseTreeAnalyzeAndBuild(context, (crank_talk_oop_t)node->stepExpression, buildContextValue);

    // Alloca the current number
    gcFrame.currentNumberVariable = (crank_talk_HIRValue_t*)crank_talk_HIRBuilder_alloca(context, builder->allocaBuilder, (crank_talk_oop_t)node->super.sourcePosition);
    crank_talk_HIRBuilder_variableStore(context, builder, gcFrame.currentNumberVariable, gcFrame.startValue, (crank_talk_oop_t)node->super.sourcePosition);

    // Create the basic blocks.
    gcFrame.loopHeader = crank_talk_HIRBasicBlock_createNamed(context, "toDoHeader");
    gcFrame.loopBody = crank_talk_HIRBasicBlock_createNamed(context, "toDoBody");
    gcFrame.loopMerge = crank_talk_HIRBasicBlock_createNamed(context, "toDoMerge");

    // Loop header
    crank_talk_HIRBuilder_branch(context, builder, gcFrame.loopHeader, (crank_talk_oop_t)node->super.sourcePosition);
    builder->basicBlock = gcFrame.loopHeader;
    
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.loopHeader);

    crank_talk_oop_t currentNumber = crank_talk_HIRBuilder_variableLoad(context, builder, gcFrame.currentNumberVariable, (crank_talk_oop_t)node->super.sourcePosition);

    {
        crank_talk_oop_t lessOrEqualsSelector = crank_talk_HIRConstantLiteralValue_create(context, context->roots.lessOrEqualsSymbol, (crank_talk_oop_t)node->super.sourcePosition);
        crank_talk_oop_t continueCondition = crank_talk_HIRBuilder_send(context, builder, (crank_talk_HIRValue_t*)lessOrEqualsSelector, (crank_talk_Array_t*)crank_talk_Array_WithWith(context, currentNumber, (crank_talk_oop_t)gcFrame.stopValue), (crank_talk_oop_t)node->super.sourcePosition);
        crank_talk_HIRBuilder_conditionalBranch(context, builder, (crank_talk_HIRValue_t*)continueCondition, gcFrame.loopBody, gcFrame.loopMerge, (crank_talk_oop_t)node->super.sourcePosition);
    }

    // Loop body
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.loopBody);
    builder->basicBlock = gcFrame.loopBody;

    // Cull inline block.
    crank_talk_ParseTree_cullInlineBlock(context, node->blockExpression, buildContextValue, (crank_talk_HIRValue_t*)currentNumber);

    // Increment
    {
        crank_talk_oop_t plusSelector = crank_talk_HIRConstantLiteralValue_create(context, context->roots.plusSymbol, (crank_talk_oop_t)node->super.sourcePosition);
        crank_talk_oop_t incrementedValue = crank_talk_HIRBuilder_send(context, builder, (crank_talk_HIRValue_t*)plusSelector, (crank_talk_Array_t*)crank_talk_Array_WithWith(context, currentNumber, (crank_talk_oop_t)gcFrame.stepValue), (crank_talk_oop_t)node->super.sourcePosition);
        crank_talk_HIRBuilder_variableStore(context,builder, gcFrame.currentNumberVariable, (crank_talk_HIRValue_t*)incrementedValue, (crank_talk_oop_t)node->super.sourcePosition);
    }

    crank_talk_HIRBuilder_branch(context, builder, gcFrame.loopHeader, (crank_talk_oop_t)node->super.sourcePosition);

    // Loop merge
    builder->basicBlock = gcFrame.loopMerge;
    crank_talk_HIRFunction_addBasicBlock(builder->hirFunction, gcFrame.loopMerge);

    CRANK_TALK_STACKFRAME_POP_GC_ROOTS(gcFrameRecord);
    return crank_talk_HIRConstantLiteralValue_create(context, crank_talk_oop_nil(), (crank_talk_oop_t)node->super.sourcePosition);
}

void
crank_talk_registerAnalysisAndBuildPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LiteralVariableClass, "analyzeAndBuildAssignment:withValue:with:", 4, "LiteralVariable::analyzeAndBuildAssignment:withValue:with:", crank_talk_LiteralVariable_analyzeAndBuildAssignmentWithWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRVariableClass, "analyzeAndBuildAssignment:withValue:with:", 4, "HIRVariable::analyzeAndBuildAssignment:withValue:with:", crank_talk_HIRVariable_analyzeAndBuildAssignmentWithWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRAllocaInstructionClass, "analyzeAndBuildAssignment:withValue:with:", 4, "HIRAllocaInstruction::analyzeAndBuildAssignment:withValue:with:", crank_talk_HIRAlloca_analyzeAndBuildAssignmentWithWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRCaptureClass, "analyzeAndBuildAssignment:withValue:with:", 4, "HIRCapture::analyzeAndBuildAssignment:withValue:with:", crank_talk_HIRCapture_analyzeAndBuildAssignmentWithWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SlotClass, "analyzeAndBuildAssignment:withValue:with:", 4, "Slot::analyzeAndBuildAssignment:withValue:with:", crank_talk_Slot_analyzeAndBuildAssignmentWithWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LiteralVariableClass, "analyzeAndBuildIdentifierReference:with:", 3, "LiteralVariable::analyzeAndBuildIdentifierReference:with:", crank_talk_LiteralVariable_analyzeAndBuildIdentifierReferenceWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRVariableClass, "analyzeAndBuildIdentifierReference:with:", 3, "HIRVariable::analyzeAndBuildIdentifierReference:with:", crank_talk_HIRVariable_analyzeAndBuildIdentifierReferenceWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRValueClass, "analyzeAndBuildIdentifierReference:with:", 3, "HIRValue::analyzeAndBuildIdentifierReference:with:", crank_talk_HIRValue_analyzeAndBuildIdentifierReferenceWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRAllocaInstructionClass, "analyzeAndBuildIdentifierReference:with:", 3, "HIRAllocaInstruction::analyzeAndBuildIdentifierReference:with:", crank_talk_HIRAllocaInstruction_analyzeAndBuildIdentifierReferenceWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.HIRCaptureClass, "analyzeAndBuildIdentifierReference:with:", 3, "HIRCapture::analyzeAndBuildIdentifierReference:with:", crank_talk_HIRCaptureInstruction_analyzeAndBuildIdentifierReferenceWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SlotClass, "analyzeAndBuildIdentifierReference:with:", 3, "Slot::analyzeAndBuildIdentifierReference:with:", crank_talk_Slot_analyzeAndBuildIdentifierReferenceWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeAssignmentNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeAssignmentNode::analyzeAndBuildWith:", crank_talk_ParseTreeAssignmentNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeBlockNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeBlockNode::analyzeAndBuildWith:", crank_talk_ParseTreeBlockNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeIdentifierReferenceNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeIdentifierReferenceNode::analyzeAndBuildWith:", crank_talk_ParseTreeIdentifierReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeLiteralNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeLiteralNode::analyzeAndBuildWith:", crank_talk_ParseTreeLiteralNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeNilReferenceNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeNilReferenceNode::analyzeAndBuildWith:", crank_talk_ParseTreeNilReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeFalseReferenceNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeFalseReferenceNode::analyzeAndBuildWith:", crank_talk_ParseTreeFalseReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeTrueReferenceNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeTrueReferenceNode::analyzeAndBuildWith:", crank_talk_ParseTreeTrueReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSelfReferenceNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeSelfReferenceNode::analyzeAndBuildWith:", crank_talk_ParseTreeSelfReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSuperReferenceNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeSuperReferenceNode::analyzeAndBuildWith:", crank_talk_ParseTreeSuperReferenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeThisContextReferenceNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeThisContextReferenceNode::analyzeAndBuildWith:", crank_talk_ParseTreeThisContextReferenceNode_analyzeAndBuildWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageCascadeNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeMessageCascadeNode::analyzeAndBuildWith:", crank_talk_ParseTreeMessageCascadeNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageSendNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeMessageSendNode::analyzeAndBuildWith:", crank_talk_ParseTreeMessageSendNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeReturnNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeReturnNode::analyzeAndBuildWith:", crank_talk_ParseTreeReturnNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSequenceNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeSequenceNode::analyzeAndBuildWith:", crank_talk_ParseTreeSequenceNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeVariableDefinitionNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeVariableDefinitionNode::analyzeAndBuildWith:", crank_talk_ParseTreeVariableDefinitionNode_analyzeAndBuildWith);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeCullBlockNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeCullBlockNode::analyzeAndBuildWith:", crank_talk_ParseTreeCullBlockNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeIfSelectionNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeIfSelectionNode::analyzeAndBuildWith:", crank_talk_ParseTreeIfSelectionNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeWhileNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeWhileNode::analyzeAndBuildWith:", crank_talk_ParseTreeWhileNode_analyzeAndBuildWith);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeDoWhileNodeClass, "analyzeAndBuildWith:", 2, "ParseTreeDoWhileNode::analyzeAndBuildWith:", crank_talk_ParseTreeDoWhileNode_analyzeAndBuildWith);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.ParseTreeToByDoNodeClass, "analyzeAndBuildWith:", 2, crank_talk_ToByDoNode_analyzeAndBuildWith);
}

