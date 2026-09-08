#include "crank-talk/environment.h"
#include "crank-talk/context.h"
#include "crank-talk/dictionary.h"

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
