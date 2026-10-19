#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static crank_talk_oop_t
crank_talk_Number_asFloat_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];

    double value = crank_talk_oop_numberAsFloat(self);
    return crank_talk_oop_fromFloat(context, value);
}

static crank_talk_oop_t
crank_talk_Float_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t stream = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%g", value);

    crank_talk_StringStream_insertCString(context, stream, buffer);
    if(!strchr(buffer, '.'))
        crank_talk_StringStream_insertCString(context, stream, ".0");

    return self;
}

static crank_talk_oop_t
crank_talk_Float_negated_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_oop_fromFloat(context, -value);
}

static crank_talk_oop_t
crank_talk_Float_sqrt_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_oop_fromFloat(context, sqrt(value));
}

static crank_talk_oop_t
crank_talk_Float_add_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_oop_fromFloat(context, value + operandValue);
}

static crank_talk_oop_t
crank_talk_Float_minus_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_oop_fromFloat(context, value - operandValue);
}

static crank_talk_oop_t
crank_talk_Float_times_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_oop_fromFloat(context, value * operandValue);
}

static crank_talk_oop_t
crank_talk_Float_divide_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_oop_fromFloat(context, value / operandValue);
}

static crank_talk_oop_t
crank_talk_Float_equals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return  crank_talk_oop_fromBoolean(context, value == operandValue);
}

static crank_talk_oop_t
crank_talk_Float_notEquals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_oop_fromBoolean(context, value != operandValue);
}

static crank_talk_oop_t
crank_talk_Float_lessThan_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_oop_fromBoolean(context, value < operandValue);
}

static crank_talk_oop_t
crank_talk_Float_lessOrEquals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_oop_fromBoolean(context, value <= operandValue);
}

static crank_talk_oop_t
crank_talk_Float_greaterThan_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_oop_fromBoolean(context, value > operandValue);
}

static crank_talk_oop_t
crank_talk_Float_greaterOrEquals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_oop_fromBoolean(context, value >= operandValue);
}

static crank_talk_oop_t
crank_talk_Float_exp_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_oop_fromFloat(context, exp(value));
}

static crank_talk_oop_t
crank_talk_Float_ln_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_oop_fromFloat(context, log(value));
}

static crank_talk_oop_t
crank_talk_Float_cos_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_oop_fromFloat(context, cos(value));
}

static crank_talk_oop_t
crank_talk_Float_sin_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_oop_fromFloat(context, sin(value));
}

static crank_talk_oop_t
crank_talk_SmallInteger_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t stream = arguments[1];

    long long int value = (long long int)crank_talk_oop_decodeSmallInteger(self);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%lld", value);

    crank_talk_StringStream_insertCString(context, stream, buffer);
    return self;
}

static void
crank_talk_SmallInteger_negated_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_encodeSmallInteger(-value));
}

static crank_talk_oop_t
crank_talk_SmallInteger_add_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_encodeSmallInteger(value + operandValue);
}

static crank_talk_oop_t
crank_talk_SmallInteger_minus_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_encodeSmallInteger(value - operandValue);
}

static crank_talk_oop_t
crank_talk_SmallInteger_times_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_encodeSmallInteger(value * operandValue);
}

static crank_talk_oop_t
crank_talk_SmallInteger_divide_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_encodeSmallInteger(value / operandValue);
}

static crank_talk_oop_t
crank_talk_SmallInteger_equals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_fromBoolean(context, value == operandValue);
}

static crank_talk_oop_t
crank_talk_SmallInteger_notEquals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_fromBoolean(context, value != operandValue);
}

static crank_talk_oop_t
crank_talk_SmallInteger_lessThan_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_fromBoolean(context, value < operandValue);
}

static crank_talk_oop_t
crank_talk_SmallInteger_lessOrEquals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_fromBoolean(context, value <= operandValue);
}

static crank_talk_oop_t
crank_talk_SmallInteger_greaterThan_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_fromBoolean(context, value > operandValue);
}

static crank_talk_oop_t
crank_talk_SmallInteger_greaterOrEquals_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 1);

    crank_talk_oop_t self = arguments[0];
    crank_talk_oop_t operand = arguments[1];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_oop_fromBoolean(context, value >= operandValue);
}

void
crank_talk_registerNumbersPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.NumberClass, "asFloat", 1, crank_talk_Number_asFloat_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "printOn:", 2, crank_talk_Float_printOn_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "negated", 1, crank_talk_Float_negated_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "sqrt", 1, crank_talk_Float_sqrt_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "+", 2, crank_talk_Float_add_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "-", 2, crank_talk_Float_minus_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "*", 2, crank_talk_Float_times_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "/", 2, crank_talk_Float_divide_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "=",  2, crank_talk_Float_equals_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "~=", 2, crank_talk_Float_notEquals_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "<",  2, crank_talk_Float_lessThan_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "<=", 2, crank_talk_Float_lessOrEquals_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, ">",  2, crank_talk_Float_greaterThan_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, ">=", 2, crank_talk_Float_greaterOrEquals_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "exp", 1, crank_talk_Float_exp_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "ln",  1, crank_talk_Float_ln_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "cos", 1, crank_talk_Float_cos_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.FloatClass, "sin", 1, crank_talk_Float_sin_primitive);
    
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, "printOn:", 2, crank_talk_SmallInteger_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "negated", 1, "SmallInteger::negated", crank_talk_SmallInteger_negated_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, "+", 2, crank_talk_SmallInteger_add_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, "-", 2, crank_talk_SmallInteger_minus_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, "*", 2, crank_talk_SmallInteger_times_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, "//", 2, crank_talk_SmallInteger_divide_primitive);

    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, "=", 2, crank_talk_SmallInteger_equals_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, "~=", 2, crank_talk_SmallInteger_notEquals_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, "<", 2, crank_talk_SmallInteger_lessThan_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, "<=", 2, crank_talk_SmallInteger_lessOrEquals_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, ">", 2, crank_talk_SmallInteger_greaterThan_primitive);
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SmallIntegerClass, ">=", 2, crank_talk_SmallInteger_greaterOrEquals_primitive);
}
