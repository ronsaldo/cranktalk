#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <stdio.h>
#include <string.h>

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

void
crank_talk_registerNumbersPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.FloatClass, "printOn:", 2, crank_talk_Float_printOn_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SmallIntegerClass, "printOn:", 2, crank_talk_SmallInteger_printOn_primitive);
}