#include "assertions.h"
#include "crank-talk/context.h"
#include "crank-talk/eval.h"

void analysisAndEvaluation_runTests(void)
{
    crank_talk_VMContext_t *context = crank_talk_vm_createContext();

    // Empty string
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "");
        AssertEquals(crank_talk_oop_nil(), value);
    }

    crank_talk_vm_destroyContext(context);
}