#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static void
crank_talk_Number_asFloat_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    double value = crank_talk_oop_numberAsFloat(self);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, value));
}

static void
crank_talk_Float_printOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%g", value);

    crank_talk_StringStream_insertCString(primContext->context, stream, buffer);
    if(!strchr(buffer, '.'))
        crank_talk_StringStream_insertCString(primContext->context, stream, ".0");

    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_Float_negated_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, -value));
}

static void
crank_talk_Float_sqrt_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, sqrt(value)));
}

static void
crank_talk_Float_add_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, value + operandValue));
}

static void
crank_talk_Float_minus_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, value - operandValue));
}

static void
crank_talk_Float_times_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, value * operandValue));
}

static void
crank_talk_Float_divide_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, value / operandValue));
}

static void
crank_talk_Float_equals_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value == operandValue));
}

static void
crank_talk_Float_notEquals_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value != operandValue));
}

static void
crank_talk_Float_lessThan_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value < operandValue));
}

static void
crank_talk_Float_lessOrEquals_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value <= operandValue));
}

static void
crank_talk_Float_greaterThan_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value > operandValue));
}

static void
crank_talk_Float_greaterOrEquals_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    double value = crank_talk_oop_asFloat(self);
    double operandValue = crank_talk_oop_numberAsFloat(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value >= operandValue));
}

static void
crank_talk_Float_exp_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, exp(value)));
}

static void
crank_talk_Float_ln_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, log(value)));
}

static void
crank_talk_Float_cos_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, cos(value)));
}

static void
crank_talk_Float_sin_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    double value = crank_talk_oop_asFloat(self);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromFloat(primContext->context, sin(value)));
}

static void
crank_talk_SmallInteger_printOn_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t stream = primContext->arguments[1];

    long long int value = (long long int)crank_talk_oop_decodeSmallInteger(self);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%lld", value);

    crank_talk_StringStream_insertCString(primContext->context, stream, buffer);
    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_SmallInteger_negated_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_encodeSmallInteger(-value));
}

static void
crank_talk_SmallInteger_add_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_encodeSmallInteger(value + operandValue));
}

static void
crank_talk_SmallInteger_minus_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_encodeSmallInteger(value - operandValue));
}

static void
crank_talk_SmallInteger_times_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_encodeSmallInteger(value * operandValue));
}

static void
crank_talk_SmallInteger_divide_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);

    // Division by zero.
    if (operandValue == 0) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_oop_encodeSmallInteger(value / operandValue));
}

static void
crank_talk_SmallInteger_modulo_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);

    // Division by zero.
    if (operandValue == 0) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_oop_encodeSmallInteger(value % operandValue));
}

static void
crank_talk_SmallInteger_divideFraction_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);

    // Division by zero.
    if (operandValue == 0) return crank_talk_Primitive_fail(primContext);

    // Is the division exact?
    if (value % operandValue != 0) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_oop_encodeSmallInteger(value / operandValue));
}

static void
crank_talk_SmallInteger_equals_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value == operandValue));
}

static void
crank_talk_SmallInteger_notEquals_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value != operandValue));
}

static void
crank_talk_SmallInteger_lessThan_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value < operandValue));
}

static void
crank_talk_SmallInteger_lessOrEquals_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value <= operandValue));
}

static void
crank_talk_SmallInteger_greaterThan_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value > operandValue));
}

static void
crank_talk_SmallInteger_greaterOrEquals_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];
    crank_talk_oop_t operand = primContext->arguments[1];

    if(!crank_talk_oop_isSmallInteger(operand)) return crank_talk_Primitive_fail(primContext);

    intptr_t value = crank_talk_oop_decodeSmallInteger(self);
    intptr_t operandValue = crank_talk_oop_decodeSmallInteger(operand);
    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromBoolean(primContext->context, value >= operandValue));
}

void
crank_talk_registerNumbersPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.NumberClass, "asFloat", 1, "Number::asFloat", crank_talk_Number_asFloat_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "printOn:", 2, "Float::printOn:", crank_talk_Float_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "negated", 1, "Float::negated", crank_talk_Float_negated_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "sqrt",    1, "Float::sqrt", crank_talk_Float_sqrt_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "+", 2, "Float::+", crank_talk_Float_add_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "-", 2, "Float::-", crank_talk_Float_minus_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "*", 2, "Float::*", crank_talk_Float_times_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "/", 2, "Float::/", crank_talk_Float_divide_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "=",  2, "Float::=",  crank_talk_Float_equals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "~=", 2, "Float::~=", crank_talk_Float_notEquals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "<",  2, "Float::<",  crank_talk_Float_lessThan_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "<=", 2, "Float::<=", crank_talk_Float_lessOrEquals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, ">",  2, "Float::>",  crank_talk_Float_greaterThan_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, ">=", 2, "Float::>=", crank_talk_Float_greaterOrEquals_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "exp", 1, "Float::exp", crank_talk_Float_exp_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "ln",  1, "Float::ln",  crank_talk_Float_ln_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "cos", 1, "Float::cos", crank_talk_Float_cos_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "sin", 1, "Float::sin", crank_talk_Float_sin_primitive);
    
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "printOn:", 2, "SmallInteger::printOn:", crank_talk_SmallInteger_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "negated", 1, "SmallInteger::negated", crank_talk_SmallInteger_negated_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "+", 2,  "SmallInteger::+",  crank_talk_SmallInteger_add_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "-", 2,  "SmallInteger::-",  crank_talk_SmallInteger_minus_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "*", 2,  "SmallInteger::*",  crank_talk_SmallInteger_times_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "//", 2, "SmallInteger:://", crank_talk_SmallInteger_divide_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "\\\\", 2, "SmallInteger::\\", crank_talk_SmallInteger_modulo_primitive);
crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "/", 2,  "SmallInteger::/",  crank_talk_SmallInteger_divideFraction_primitive);
    
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "=",  2, "SmallInteger::=",  crank_talk_SmallInteger_equals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "~=", 2, "SmallInteger::~=", crank_talk_SmallInteger_notEquals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "<",  2, "SmallInteger::<",  crank_talk_SmallInteger_lessThan_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "<=", 2, "SmallInteger::<=", crank_talk_SmallInteger_lessOrEquals_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, ">",  2, "SmallInteger::>",  crank_talk_SmallInteger_greaterThan_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, ">=", 2, "SmallInteger::>=", crank_talk_SmallInteger_greaterOrEquals_primitive);
}
