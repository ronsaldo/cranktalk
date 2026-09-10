#include "crank-talk/environment.h"
#include "crank-talk/context.h"
#include "crank-talk/dictionary.h"
#include "crank-talk/message.h"

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

CRANK_TALK_API crank_talk_oop_t
crank_talk_Environment_lookupSymbolRecursively(crank_talk_VMContext_t *context, crank_talk_oop_t environment, crank_talk_oop_t symbol)
{
    return crank_talk_send1(context, context->roots.lookupSymbolRecursively, environment, symbol);
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

void
crank_talk_registerEnvironmentPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.EmptyEnvironmentClass, "lookupSymbolRecursively:", 2, crank_talk_EmptyEnvironment_lookupSymbolRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SystemEnvironmentEnvironmentClass, "lookupSymbolRecursively:", 2, crank_talk_SystemEnvironmentEnvironment_lookupSymbolRecursively);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.LexicalEnvironmentClass, "lookupSymbolRecursively:", 2,  crank_talk_LexicalEnvironment_lookupSymbolRecursively);

}
