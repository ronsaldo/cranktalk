#include "crank-talk/time.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"

#ifdef _WIN32
#   error TODO: implement crank_talk_getMicrosecondClockValue
#else
#include <time.h>

CRANK_TALK_API int64_t
crank_talk_getMicrosecondClockValue(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec*1000000 + (int64_t)ts.tv_nsec / 1000;
}

#endif

static void
crank_talk_Time_microsecondClockValue_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1 ) return crank_talk_Primitive_fail(primContext);

    return crank_talk_Primitive_return(primContext, crank_talk_oop_fromInt64(primContext->context, crank_talk_getMicrosecondClockValue()));
}

void
crank_talk_registerTimePrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.TimeClass), "microsecondClockValue", 1, "Time::microsecondClockValue", crank_talk_Time_microsecondClockValue_primitive);
}