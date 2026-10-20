#include "crank-talk/environment.h"
#include "crank-talk/behavior.h"
#include "crank-talk/context.h"
#include "crank-talk/dictionary.h"
#include "crank-talk/hir.h"
#include "crank-talk/message.h"
#include "crank-talk/ordered-collection.h"
#include <stdlib.h>

CRANK_TALK_API crank_talk_oop_t
crank_talk_LexicalEnvironment_create(crank_talk_VMContext_t *context, crank_talk_oop_t parentEnvironment)
{
    crank_talk_oop_t lexicalEnvironmentValue = CrankAllocateValuesObjectWithClassName(context, LexicalEnvironment);
    crank_talk_LexicalEnvironment_t *lexicalEnvironment = (crank_talk_LexicalEnvironment_t *)lexicalEnvironmentValue;
    lexicalEnvironment->parent = parentEnvironment;
    lexicalEnvironment->symbolTable = crank_talk_IdentityDictionary_create(context);
    return lexicalEnvironmentValue;
}

CRANK_TALK_API void
crank_talk_LexicalEnvironment_setSymbolBinding(crank_talk_VMContext_t *context, crank_talk_oop_t environmentValue, crank_talk_oop_t symbol, crank_talk_oop_t binding)
{
    assert(!crank_talk_oop_isNil(symbol));
    crank_talk_LexicalEnvironment_t *lexicalEnvironment = (crank_talk_LexicalEnvironment_t *)environmentValue;
    crank_talk_IdentityDictionary_localAtPut(context, lexicalEnvironment->symbolTable, symbol, binding);
}

CRANK_TALK_API void
crank_talk_LexicalEnvironment_setNewSymbolBinding(crank_talk_VMContext_t *context, crank_talk_oop_t environment, crank_talk_oop_t symbol, crank_talk_oop_t binding, crank_talk_oop_t sourcePosition)
{
    // TODO: Make sure the symbol is not in the environment.
    (void)sourcePosition;
    crank_talk_LexicalEnvironment_setSymbolBinding(context, environment, symbol, binding);
}

CRANK_TALK_API void
crank_talk_LexicalEnvironment_setHIRSymbolBinding(crank_talk_VMContext_t *context, crank_talk_oop_t environmentValue, crank_talk_oop_t symbol, crank_talk_oop_t binding)
{
    assert(!crank_talk_oop_isNil(symbol));
    crank_talk_LexicalEnvironment_t *lexicalEnvironment = (crank_talk_LexicalEnvironment_t *)environmentValue;
    crank_talk_IdentityDictionary_hirVariableAtPut(context, lexicalEnvironment->symbolTable, symbol, binding);
}

CRANK_TALK_API void
crank_talk_LexicalEnvironment_setNewHIRSymbolBinding(crank_talk_VMContext_t *context, crank_talk_oop_t environment, crank_talk_oop_t symbol, crank_talk_oop_t binding, crank_talk_oop_t sourcePosition)
{
    // TODO: Make sure the symbol is not in the environment.
    (void)sourcePosition;
    crank_talk_LexicalEnvironment_setHIRSymbolBinding(context, environment, symbol, binding);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_FunctionAnalysisEnvironment_create(crank_talk_VMContext_t *context, crank_talk_oop_t parentEnvironment)
{
    crank_talk_oop_t functionEnvironmentValue = CrankAllocateValuesObjectWithClassName(context, FunctionAnalysisEnvironment);
    crank_talk_FunctionAnalysisEnvironment_t*functionEnvironment = (crank_talk_FunctionAnalysisEnvironment_t*)functionEnvironmentValue;
    functionEnvironment->super.parent = parentEnvironment;
    functionEnvironment->super.symbolTable = crank_talk_IdentityDictionary_create(context);
    functionEnvironment->captureList = crank_talk_OrderedCollection_create(context);
    functionEnvironment->captureSymbolTable = crank_talk_IdentityDictionary_create(context);
    return functionEnvironmentValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Environment_lookupSymbolRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t environment, crank_talk_oop_t symbol)
{
    return crank_talk_send1(context, context->roots.lookupSymbolRecursively, environment, symbol);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Environment_lookupReceiverRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t environment)
{
    return crank_talk_Environment_lookupSymbolRecursively(context, environment, context->roots.selfSymbol);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Environment_lookupReceiverClassRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t environment)
{
    return crank_talk_send0(context, context->roots.lookupReceiverClassRecursively, environment);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Environment_lookupIsInnerBlockRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t environment)
{
    return crank_talk_send0(context, context->roots.lookupIsInnerBlockRecursively, environment);
}

static void
crank_talk_EmptyEnvironment_lookupSymbolRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_SystemEnvironmentEnvironment_lookupSymbolRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_SystemEnvironmentEnvironment_t *environment = (crank_talk_SystemEnvironmentEnvironment_t*)primContext->arguments[0];
    crank_talk_oop_t symbol = primContext->arguments[1];

    crank_talk_oop_t binding = crank_talk_IdentityDictionary_associationAtOrNil(environment->systemEnvironment, symbol);
    if(!crank_talk_oop_isNil(binding))
        return crank_talk_Primitive_return(primContext, binding);

    crank_talk_oop_t result = crank_talk_Environment_lookupSymbolRecursively(primContext->context, environment->parent, symbol);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_LexicalEnvironment_lookupSymbolRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_LexicalEnvironment_t *environment = (crank_talk_LexicalEnvironment_t*)primContext->arguments[0];
    crank_talk_oop_t symbol = primContext->arguments[1];

    crank_talk_oop_t binding = crank_talk_IdentityDictionary_associationAtOrNil(environment->symbolTable, symbol);
    if(!crank_talk_oop_isNil(binding))
        return crank_talk_Primitive_return(primContext, binding);

    crank_talk_oop_t result = crank_talk_Environment_lookupSymbolRecursively(primContext->context, environment->parent, symbol);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_FunctionAnalysisEnvironment_lookupSymbolRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_FunctionAnalysisEnvironment_t *environment = (crank_talk_FunctionAnalysisEnvironment_t*)primContext->arguments[0];
    crank_talk_oop_t symbol = primContext->arguments[1];

    crank_talk_oop_t binding = crank_talk_IdentityDictionary_associationAtOrNil(environment->super.symbolTable, symbol);
    if(!crank_talk_oop_isNil(binding))
        return crank_talk_Primitive_return(primContext, binding);

    binding = crank_talk_IdentityDictionary_associationAtOrNil(environment->captureSymbolTable, symbol);
    if(!crank_talk_oop_isNil(binding))
        return crank_talk_Primitive_return(primContext, binding);

    if(environment->receiverClass)
    {
        crank_talk_oop_t slot = crank_talk_Behavior_lookupSlotNamed((crank_talk_oop_t)environment->receiverClass, symbol);
        if(slot)
            return crank_talk_Primitive_return(primContext, slot);
    }

    binding = crank_talk_Environment_lookupSymbolRecursively(primContext->context, environment->super.parent, symbol);
    if(crank_talk_oop_isNil(binding))
        return crank_talk_Primitive_return(primContext, binding);
    
    // Is this a HIR variable?
    if(crank_talk_oop_isKindOf(primContext->context, binding, primContext->context->roots.HIRVariableClass))
    {
        crank_talk_HIRVariable_t *hirVariable = (crank_talk_HIRVariable_t *)binding;
        
        // Is this a function local value?
        if(crank_talk_oop_isKindOf(primContext->context, hirVariable->value, primContext->context->roots.HIRFunctionLocalClass))
        {
            // Create the capture.
            crank_talk_oop_t captureValue = CrankAllocateValuesObjectWithClassName(primContext->context, HIRCapture);
            crank_talk_HIRCapture_t *capture = (crank_talk_HIRCapture_t *)captureValue;
            capture->capturedValue = (crank_talk_HIRFunctionLocal_t*)hirVariable->value;
            capture->isVariable = crank_talk_send0(primContext->context, primContext->context->roots.isHIRVariableSymbol, (crank_talk_oop_t)capture->capturedValue);

            crank_talk_OrderedCollection_add(primContext->context, environment->captureList, captureValue);
            crank_talk_IdentityDictionary_hirVariableAtPut(primContext->context, environment->captureSymbolTable, symbol, captureValue);
            crank_talk_oop_t result = crank_talk_IdentityDictionary_associationAtOrNil(environment->captureSymbolTable, symbol);
            return crank_talk_Primitive_return(primContext, result);
        }
    }

    return crank_talk_Primitive_return(primContext, binding);
}

static void
crank_talk_EmptyEnvironment_lookupReceiverClassRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_LexicalEnvironment_lookupReceiverClassRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_LexicalEnvironment_t *environment = (crank_talk_LexicalEnvironment_t*)primContext->arguments[0];

    crank_talk_oop_t result = crank_talk_Environment_lookupReceiverClassRecursively(primContext->context, environment->parent);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_FunctionAnalysisEnvironment_lookupReceiverClassRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_FunctionAnalysisEnvironment_t *environment = (crank_talk_FunctionAnalysisEnvironment_t*)primContext->arguments[0];

    if(environment->receiverClass)
        return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)environment->receiverClass);
    
    crank_talk_oop_t result = crank_talk_Environment_lookupReceiverClassRecursively(primContext->context, environment->super.parent);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_EmptyEnvironment_lookupIsInnerBlockRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    return crank_talk_Primitive_return(primContext, primContext->context->roots.falseValue);
}

static void
crank_talk_LexicalEnvironment_lookupIsInnerBlockRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_LexicalEnvironment_t *environment = (crank_talk_LexicalEnvironment_t*)primContext->arguments[0];
    crank_talk_oop_t result = crank_talk_Environment_lookupIsInnerBlockRecursively(primContext->context, environment->parent);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_FunctionAnalysisEnvironment_lookupIsInnerBlockRecursively(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_FunctionAnalysisEnvironment_t *environment = (crank_talk_FunctionAnalysisEnvironment_t*)primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, environment->isInnerBlock);
}

void
crank_talk_registerEnvironmentPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.EmptyEnvironmentClass, "lookupSymbolRecursively:", 2, "EmptyEnvironment::lookupSymbolRecursively:", crank_talk_EmptyEnvironment_lookupSymbolRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SystemEnvironmentEnvironmentClass, "lookupSymbolRecursively:", 2, "SystemEnvironmentEnvironmentClass::lookupSymbolRecursively:", crank_talk_SystemEnvironmentEnvironment_lookupSymbolRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LexicalEnvironmentClass, "lookupSymbolRecursively:", 2, "LexicalEnvironment::lookupSymbolRecursively:", crank_talk_LexicalEnvironment_lookupSymbolRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FunctionAnalysisEnvironmentClass, "lookupSymbolRecursively:", 2, "FunctionAnalysisEnvironmentClass::lookupSymbolRecursively:", crank_talk_FunctionAnalysisEnvironment_lookupSymbolRecursively);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.EmptyEnvironmentClass, "lookupReceiverClassRecursively", 1, "EmptyEnvironment::lookupReceiverClassRecursively", crank_talk_EmptyEnvironment_lookupReceiverClassRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LexicalEnvironmentClass, "lookupReceiverClassRecursively", 1, "LexicalEnvironment::lookupReceiverClassRecursively", crank_talk_LexicalEnvironment_lookupReceiverClassRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FunctionAnalysisEnvironmentClass, "lookupReceiverClassRecursively", 1, "FunctionAnalysisEnvironment::lookupReceiverClassRecursively", crank_talk_FunctionAnalysisEnvironment_lookupReceiverClassRecursively);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.EmptyEnvironmentClass, "lookupIsInnerBlockRecursively", 1, "EmptyEnvironment::lookupIsInnerBlockRecursively", crank_talk_EmptyEnvironment_lookupIsInnerBlockRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LexicalEnvironmentClass, "lookupIsInnerBlockRecursively", 1, "LexicalEnvironment::lookupIsInnerBlockRecursively", crank_talk_LexicalEnvironment_lookupIsInnerBlockRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FunctionAnalysisEnvironmentClass, "lookupIsInnerBlockRecursively", 1, "FunctionAnalysisEnvironment::lookupIsInnerBlockRecursively", crank_talk_FunctionAnalysisEnvironment_lookupIsInnerBlockRecursively);
}
