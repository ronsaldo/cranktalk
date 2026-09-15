#ifndef CRANK_TALK_MESSAGE_H
#define CRANK_TALK_MESSAGE_H

#include "object-model.h"
#include "function.h"

CRANK_TALK_API void crank_talk_Behavior_withSelectorSymbolSetMethod(crank_talk_VMContext_t *context, crank_talk_oop_t behavior, crank_talk_oop_t selector, crank_talk_oop_t method);
CRANK_TALK_API void crank_talk_Behavior_withSelectorSetMethod(crank_talk_VMContext_t *context, crank_talk_oop_t behavior, const char *selector, crank_talk_oop_t method);
CRANK_TALK_API void crank_talk_createAndSetPrimitiveMethod(crank_talk_VMContext_t *context, crank_talk_oop_t behavior, const char *selector, size_t argumentCount, crank_talk_CompiledFunctionNativeEntryPoint_t entryPoint);

CRANK_TALK_API crank_talk_oop_t crank_talk_send(crank_talk_VMContext_t *context, crank_talk_oop_t selector, size_t argumentCount, crank_talk_oop_t *arguments);

CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_send0(crank_talk_VMContext_t *context, crank_talk_oop_t selector, crank_talk_oop_t receiver)
{
    crank_talk_oop_t arguments[] = {
        receiver,
    };
    return crank_talk_send(context, selector, 1, arguments);
}

CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_send1(crank_talk_VMContext_t *context, crank_talk_oop_t selector, crank_talk_oop_t receiver, crank_talk_oop_t argument0)
{
    crank_talk_oop_t arguments[] = {
        receiver,
        argument0,
    };
    return crank_talk_send(context, selector, 2, arguments);
}

CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_send2(crank_talk_VMContext_t *context, crank_talk_oop_t selector, crank_talk_oop_t receiver, crank_talk_oop_t argument0, crank_talk_oop_t argument1)
{
    crank_talk_oop_t arguments[] = {
        receiver,
        argument0,
        argument1,
    };
    return crank_talk_send(context, selector, 3, arguments);
}

CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_send3(crank_talk_VMContext_t *context, crank_talk_oop_t selector, crank_talk_oop_t receiver, crank_talk_oop_t argument0, crank_talk_oop_t argument1, crank_talk_oop_t argument2)
{
    crank_talk_oop_t arguments[] = {
        receiver,
        argument0,
        argument1,
        argument2,
    };
    return crank_talk_send(context, selector, 4, arguments);
}

CRANK_TALK_INLINE crank_talk_oop_t
crank_talk_send4(crank_talk_VMContext_t *context, crank_talk_oop_t selector, crank_talk_oop_t receiver, crank_talk_oop_t argument0, crank_talk_oop_t argument1, crank_talk_oop_t argument2, crank_talk_oop_t argument3)
{
    crank_talk_oop_t arguments[] = {
        receiver,
        argument0,
        argument1,
        argument2,
        argument3,
    };
    return crank_talk_send(context, selector, 5, arguments);
}

#endif //SYSMEL_VM_MESSAGE_H