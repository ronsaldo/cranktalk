#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include <stdlib.h>

static void
crank_talk_True_not_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, primContext->context->roots.falseValue);
}

static void
crank_talk_False_not_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, primContext->context->roots.trueValue);
}

static void
crank_talk_True_ifTrueIfFalse_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]));
}

static void
crank_talk_True_ifFalseIfTrue_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[2]));
}

static void
crank_talk_True_ifTrue_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]));
}

static void
crank_talk_True_ifFalse_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_False_ifTrueIfFalse_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[2]));
}

static void
crank_talk_False_IfFalseIfTrue_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]));
}


static void
crank_talk_False_ifTrue_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_False_ifFalse_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]));
}

static void
crank_talk_BlockClosure_whileTrue_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    while(crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[0]) == primContext->context->roots.trueValue)
        ;

    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_BlockClosure_whileFalse_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    while(crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[0]) == primContext->context->roots.falseValue)
        ;
    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_BlockClosure_whileTrueWith_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    while(crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[0]) == primContext->context->roots.trueValue)
        crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_BlockClosure_whileFalseWith_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    while(crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[0]) == primContext->context->roots.falseValue)
        crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_BlockClosure_whileNilWith_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    while(crank_talk_oop_isNil(crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[0])))
        crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_BlockClosure_whileNotNilWith_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    while(!crank_talk_oop_isNil(crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[0])))
        crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_BlockClosure_doWhileTrueWith_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    do
    {
        crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[0]);
    } while (crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]) == primContext->context->roots.trueValue);

    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_BlockClosure_doWhileFalseWith_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    do
    {
        crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[0]);
    } while (crank_talk_send0(primContext->context, primContext->context->roots.valueSymbol, primContext->arguments[1]) == primContext->context->roots.falseValue);

    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

static void
crank_talk_Number_toDo_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 3) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t number = primContext->arguments[0];
    crank_talk_oop_t target = primContext->arguments[1];
    crank_talk_oop_t block = primContext->arguments[2];

    while (crank_talk_oop_decodeSmallInteger(number) <= crank_talk_oop_decodeSmallInteger(target))
    {
        crank_talk_send1(primContext->context, primContext->context->roots.valueWithSymbol, block, number);
        number = crank_talk_oop_encodeSmallInteger(crank_talk_oop_decodeSmallInteger(number) + 1);
    }

    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

void
crank_talk_registerControlFlowPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass,  "not", 1, "True::not",  crank_talk_True_not_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "not", 1, "False::not", crank_talk_False_not_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifTrue:ifFalse:", 3, "True::ifTrue:ifFalse:", crank_talk_True_ifTrueIfFalse_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifFalse:ifTrue:", 3, "True::ifFalse:ifTrue:", crank_talk_True_ifFalseIfTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifTrue:", 2, "True::ifTrue:", crank_talk_True_ifTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.TrueClass, "ifFalse:", 2, "True::ifFalse:", crank_talk_True_ifFalse_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifTrue:ifFalse:", 3, "False::ifTrue:ifFalse:", crank_talk_False_ifTrueIfFalse_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifFalse:ifTrue:", 3, "False::ifFalse:ifTrue:", crank_talk_False_IfFalseIfTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifTrue:", 2, "False::ifTrue:", crank_talk_False_ifTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FalseClass, "ifFalse:", 2, "False::ifFalse:", crank_talk_False_ifFalse_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileTrue", 1, "BlockClosure::whileTrue", crank_talk_BlockClosure_whileTrue_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileFalse", 1, "BlockClosure::whileFalse", crank_talk_BlockClosure_whileFalse_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileTrue:", 2, "BlockClosure::whileTrue:", crank_talk_BlockClosure_whileTrueWith_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileFalse:", 2, "BlockClosure::whileFalse:", crank_talk_BlockClosure_whileFalseWith_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileNil:", 2, "BlockClosure::whileNil:", crank_talk_BlockClosure_whileNilWith_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "whileNotNil:", 2, "BlockClosure::whileNotNil:", crank_talk_BlockClosure_whileNotNilWith_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "doWhileTrue:", 2, "BlockClosure::doWhileTrue:", crank_talk_BlockClosure_doWhileTrueWith_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.BlockClosureClass, "doWhileFalse:", 2, "BlockClosure::doWhileFalse:", crank_talk_BlockClosure_doWhileFalseWith_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.NumberClass, "to:do:", 3, "Number::to:do:", crank_talk_Number_toDo_primitive);
}
