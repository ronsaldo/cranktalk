#ifndef CRANK_TALK_FUNCTION_H
#define CRANK_TALK_FUNCTION_H

#include "object-model.h"

CRANK_TALK_API crank_talk_CompiledFunction_t *crank_talk_CompiledFunction_createForPrimitive(crank_talk_VMContext_t *context, crank_talk_oop_t name, size_t argumentCount, crank_talk_CompiledFunctionNativeEntryPoint_t entryPoint);
CRANK_TALK_API crank_talk_oop_t crank_talk_apply(crank_talk_VMContext_t *context, crank_talk_oop_t function, size_t argumentCount, crank_talk_oop_t *arguments);

CRANK_TALK_API void crank_talk_CompiledFunction_ensureAnalysis(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction);
CRANK_TALK_API void crank_talk_Functional_dumpToConsole(crank_talk_VMContext_t *context, crank_talk_oop_t functional);

CRANK_TALK_API crank_talk_oop_t crank_talk_CompiledFunction_getSourcePositionAtPC(crank_talk_VMContext_t *context, crank_talk_CompiledFunction_t *compiledFunction, size_t pc);

CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_apply1(crank_talk_VMContext_t *context, crank_talk_oop_t function, crank_talk_oop_t argument)
{
    crank_talk_oop_t arguments[] = {
        argument,
    };
    return crank_talk_apply(context, function, 1, arguments);
}

#endif //CRANK_TALK_FUNCTION_H