#include "crank-talk/context.h"

void scanner_runTests(void)
{
    crank_talk_VMContext_t *context = crank_talk_vm_createContext();
    crank_talk_vm_destroyContext(context);
}
