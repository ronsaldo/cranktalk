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

static crank_talk_oop_t
crank_talk_EmptyEnvironment_lookupSymbolRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_SystemEnvironmentEnvironment_lookupSymbolRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_SystemEnvironmentEnvironment_t *environment = (crank_talk_SystemEnvironmentEnvironment_t*)arguments[0];
    crank_talk_oop_t symbol = arguments[1];

    crank_talk_oop_t binding = crank_talk_IdentityDictionary_associationAtOrNil(environment->systemEnvironment, symbol);
    if(!crank_talk_oop_isNil(binding))
        return binding;

    return crank_talk_Environment_lookupSymbolRecursively(context, environment->parent, symbol);
}

static crank_talk_oop_t
crank_talk_LexicalEnvironment_lookupSymbolRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_LexicalEnvironment_t *environment = (crank_talk_LexicalEnvironment_t*)arguments[0];
    crank_talk_oop_t symbol = arguments[1];

    crank_talk_oop_t binding = crank_talk_IdentityDictionary_associationAtOrNil(environment->symbolTable, symbol);
    if(!crank_talk_oop_isNil(binding))
        return binding;

    return crank_talk_Environment_lookupSymbolRecursively(context, environment->parent, symbol);
}

static crank_talk_oop_t
crank_talk_FunctionAnalysisEnvironment_lookupSymbolRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_FunctionAnalysisEnvironment_t *environment = (crank_talk_FunctionAnalysisEnvironment_t*)arguments[0];
    crank_talk_oop_t symbol = arguments[1];

    crank_talk_oop_t binding = crank_talk_IdentityDictionary_associationAtOrNil(environment->super.symbolTable, symbol);
    if(!crank_talk_oop_isNil(binding))
        return binding;

    binding = crank_talk_IdentityDictionary_associationAtOrNil(environment->captureSymbolTable, symbol);
    if(!crank_talk_oop_isNil(binding))
        return binding;

    if(environment->receiverClass)
    {
        crank_talk_oop_t slot = crank_talk_Behavior_lookupSlotNamed((crank_talk_oop_t)environment->receiverClass, symbol);
        if(slot)
            return slot;
    }

    binding = crank_talk_Environment_lookupSymbolRecursively(context, environment->super.parent, symbol);
    if(crank_talk_oop_isNil(binding))
        return binding;

    // Is this a HIR variable?
    if(crank_talk_oop_isKindOf(context, binding, context->roots.HIRVariableClass))
    {
        crank_talk_HIRVariable_t *hirVariable = (crank_talk_HIRVariable_t *)binding;
        
        // Is this a function local value?
        if(crank_talk_oop_isKindOf(context, hirVariable->value, context->roots.HIRFunctionLocalClass))
        {
            // Create the capture.
            crank_talk_oop_t captureValue = CrankAllocateValuesObjectWithClassName(context, HIRCapture);
            crank_talk_HIRCapture_t *capture = (crank_talk_HIRCapture_t *)captureValue;
            capture->capturedValue = (crank_talk_HIRFunctionLocal_t*)hirVariable->value;

            crank_talk_OrderedCollection_add(context, environment->captureList, captureValue);
            crank_talk_IdentityDictionary_hirVariableAtPut(context, environment->captureSymbolTable, symbol, captureValue);
            return crank_talk_IdentityDictionary_associationAtOrNil(environment->captureSymbolTable, symbol);
        }
    }

    return binding;
}

static crank_talk_oop_t
crank_talk_EmptyEnvironment_lookupReceiverClassRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount >= 1);
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_LexicalEnvironment_lookupReceiverClassRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);
    crank_talk_LexicalEnvironment_t *environment = (crank_talk_LexicalEnvironment_t*)arguments[0];

    return crank_talk_Environment_lookupReceiverClassRecursively(context, environment->parent);
}

static crank_talk_oop_t
crank_talk_FunctionAnalysisEnvironment_lookupReceiverClassRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);
    crank_talk_FunctionAnalysisEnvironment_t *environment = (crank_talk_FunctionAnalysisEnvironment_t*)arguments[0];

    if(environment->receiverClass)
        return (crank_talk_oop_t)environment->receiverClass;
    return crank_talk_Environment_lookupReceiverClassRecursively(context, environment->super.parent);
}

//crank_talk_createAndSetPrimitiveMethod(context, context->roots.EmptyEnvironmentClass, "lookupIsInnerBlockRecursively", 1, crank_talk_EmptyEnvironment_lookupIsInnerBlockRecursively);
//crank_talk_createAndSetPrimitiveMethod(context, context->roots.LexicalEnvironmentClass, "lookupIsInnerBlockRecursively", 1, crank_talk_LexicalEnvironment_lookupIsInnerBlockRecursively);
//crank_talk_createAndSetPrimitiveMethod(context, context->roots.FunctionAnalysisEnvironmentClass, "lookupIsInnerBlockRecursively", 1,  crank_talk_FunctionAnalysisEnvironment_lookupIsInnerBlockRecursively);

static crank_talk_oop_t
crank_talk_EmptyEnvironment_lookupIsInnerBlockRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    (void)arguments;
    assert(argumentCount >= 1);
    return context->roots.falseValue;
}

static crank_talk_oop_t
crank_talk_LexicalEnvironment_lookupIsInnerBlockRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);
    crank_talk_LexicalEnvironment_t *environment = (crank_talk_LexicalEnvironment_t*)arguments[0];

    return crank_talk_Environment_lookupIsInnerBlockRecursively(context, environment->parent);
}

static crank_talk_oop_t
crank_talk_FunctionAnalysisEnvironment_lookupIsInnerBlockRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);
    crank_talk_FunctionAnalysisEnvironment_t *environment = (crank_talk_FunctionAnalysisEnvironment_t*)arguments[0];
    return environment->isInnerBlock;
}

void
crank_talk_registerEnvironmentPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.EmptyEnvironmentClass, "lookupSymbolRecursively:", 2, crank_talk_EmptyEnvironment_lookupSymbolRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SystemEnvironmentEnvironmentClass, "lookupSymbolRecursively:", 2, crank_talk_SystemEnvironmentEnvironment_lookupSymbolRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LexicalEnvironmentClass, "lookupSymbolRecursively:", 2,  crank_talk_LexicalEnvironment_lookupSymbolRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FunctionAnalysisEnvironmentClass, "lookupSymbolRecursively:", 2,  crank_talk_FunctionAnalysisEnvironment_lookupSymbolRecursively);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.EmptyEnvironmentClass, "lookupReceiverClassRecursively", 1, crank_talk_EmptyEnvironment_lookupReceiverClassRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LexicalEnvironmentClass, "lookupReceiverClassRecursively", 1, crank_talk_LexicalEnvironment_lookupReceiverClassRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FunctionAnalysisEnvironmentClass, "lookupReceiverClassRecursively", 1,  crank_talk_FunctionAnalysisEnvironment_lookupReceiverClassRecursively);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.EmptyEnvironmentClass, "lookupIsInnerBlockRecursively", 1, crank_talk_EmptyEnvironment_lookupIsInnerBlockRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LexicalEnvironmentClass, "lookupIsInnerBlockRecursively", 1, crank_talk_LexicalEnvironment_lookupIsInnerBlockRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FunctionAnalysisEnvironmentClass, "lookupIsInnerBlockRecursively", 1,  crank_talk_FunctionAnalysisEnvironment_lookupIsInnerBlockRecursively);
}
