#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include <stdlib.h>

static crank_talk_oop_t
crank_talk_True_not_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    return context->roots.falseValue;
}

static crank_talk_oop_t
crank_talk_False_not_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    return context->roots.trueValue;
}

static crank_talk_oop_t
crank_talk_True_ifTrueIfFalse_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 3);

    return crank_talk_send0(context, context->roots.valueSymbol, arguments[1]);
}

static crank_talk_oop_t
crank_talk_True_ifFalseIfTrue_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 3);

    return crank_talk_send0(context, context->roots.valueSymbol, arguments[2]);
}

static crank_talk_oop_t
crank_talk_True_ifTrue_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    return crank_talk_send0(context, context->roots.valueSymbol, arguments[1]);
}

static crank_talk_oop_t
crank_talk_True_ifFalse_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_False_ifTrueIfFalse_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 3);

    return crank_talk_send0(context, context->roots.valueSymbol, arguments[2]);
}

static crank_talk_oop_t
crank_talk_False_IfFalseIfTrue_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 3);

    return crank_talk_send0(context, context->roots.valueSymbol, arguments[1]);
}


static crank_talk_oop_t
crank_talk_False_ifTrue_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_False_ifFalse_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    return crank_talk_send0(context, context->roots.valueSymbol, arguments[1]);
}

static crank_talk_oop_t
crank_talk_BlockClosure_whileTrue_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    while(crank_talk_send0(context, context->roots.valueSymbol, arguments[0]) == context->roots.trueValue)
        ;
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_BlockClosure_whileFalse_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    while(crank_talk_send0(context, context->roots.valueSymbol, arguments[0]) == context->roots.falseValue)
        ;
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_BlockClosure_whileTrueWith_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    while(crank_talk_send0(context, context->roots.valueSymbol, arguments[0]) == context->roots.trueValue)
        crank_talk_send0(context, context->roots.valueSymbol, arguments[1]);
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_BlockClosure_whileFalseWith_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    while(crank_talk_send0(context, context->roots.valueSymbol, arguments[0]) == context->roots.falseValue)
        crank_talk_send0(context, context->roots.valueSymbol, arguments[1]);
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_BlockClosure_whileNilWith_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    while(crank_talk_oop_isNil(crank_talk_send0(context, context->roots.valueSymbol, arguments[0])))
        crank_talk_send0(context, context->roots.valueSymbol, arguments[1]);
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_BlockClosure_whileNotNilWith_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    while(!crank_talk_oop_isNil(crank_talk_send0(context, context->roots.valueSymbol, arguments[0])))
        crank_talk_send0(context, context->roots.valueSymbol, arguments[1]);
    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_BlockClosure_doWhileTrueWith_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    do
    {
        crank_talk_send0(context, context->roots.valueSymbol, arguments[0]);
    } while (crank_talk_send0(context, context->roots.valueSymbol, arguments[1]) == context->roots.trueValue);

    return crank_talk_oop_nil();
}

static crank_talk_oop_t
crank_talk_BlockClosure_doWhileFalseWith_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    do
    {
        crank_talk_send0(context, context->roots.valueSymbol, arguments[0]);
    } while (crank_talk_send0(context, context->roots.valueSymbol, arguments[1]) == context->roots.falseValue);

    return crank_talk_oop_nil();
}

void
crank_talk_registerControlFlowPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "not", 1, crank_talk_True_not_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "not", 1, crank_talk_False_not_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifTrue:ifFalse:", 3, crank_talk_True_ifTrueIfFalse_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifFalse:ifTrue:", 3, crank_talk_True_ifFalseIfTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifTrue:", 2, crank_talk_True_ifTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifFalse:", 2, crank_talk_True_ifFalse_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifTrue:ifFalse:", 3, crank_talk_False_ifTrueIfFalse_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifFalse:ifTrue:", 3, crank_talk_False_IfFalseIfTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifTrue:", 2, crank_talk_False_ifTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifFalse:", 2, crank_talk_False_ifFalse_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileTrue", 1, crank_talk_BlockClosure_whileTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileFalse", 1, crank_talk_BlockClosure_whileFalse_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileTrue:", 2, crank_talk_BlockClosure_whileTrueWith_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileFalse:", 2, crank_talk_BlockClosure_whileFalseWith_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileNil:", 2, crank_talk_BlockClosure_whileNilWith_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileNotNil:", 2, crank_talk_BlockClosure_whileNotNilWith_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "doWhileTrue:", 2, crank_talk_BlockClosure_doWhileTrueWith_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "doWhileFalse:", 2, crank_talk_BlockClosure_doWhileFalseWith_primitive);
}
