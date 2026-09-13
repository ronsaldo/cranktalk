#include "crank-talk/context.h"
#include "crank-talk/message.h"

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

void
crank_talk_registerControlFlowPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifTrue:ifFalse:", 3, crank_talk_True_ifTrueIfFalse_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifFalse:ifTrue:", 3, crank_talk_True_ifFalseIfTrue_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifTrue:ifFalse:", 3, crank_talk_False_ifTrueIfFalse_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifFalse:ifTrue:", 3, crank_talk_False_IfFalseIfTrue_primitive);
}
