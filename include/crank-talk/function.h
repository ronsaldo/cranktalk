#ifndef CRANK_TALK_FUNCTION_H
#define CRANK_TALK_FUNCTION_H

#include "object-model.h"

CRANK_TALK_API crank_talk_CompiledFunction_t *crank_talk_CompiledFunction_createForPrimitive(crank_talk_VMContext_t *context, crank_talk_oop_t name, size_t argumentCount, crank_talk_CompiledFunctionNativeEntryPoint_t entryPoint);
CRANK_TALK_API crank_talk_oop_t crank_talk_apply(crank_talk_VMContext_t *context, crank_talk_oop_t function, size_t argumentCount, crank_talk_oop_t *arguments);

#endif //CRANK_TALK_FUNCTION_H