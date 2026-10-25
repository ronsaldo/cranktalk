#include "crank-talk/random.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"

static void
crank_talk_Random_privateNext_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_Random_t *self = (crank_talk_Random_t *)primContext->arguments[0];
    uint32_t seed = (uint32_t)crank_talk_oop_decodeSmallInteger(self->seed);

    // See the glibc constants for https://en.wikipedia.org/wiki/Linear_congruential_generator [October 2026]
    uint32_t newSeed = (seed*1103515245 + 12345) & 0x7FFFFFFF;
    self->seed = crank_talk_oop_encodeSmallInteger(newSeed);
    
    return crank_talk_Primitive_return(primContext, self->seed);
}

void crank_talk_registerRandomPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.RandomClass, "privateNext", 1, "Random::privateNext", crank_talk_Random_privateNext_primitive);
}