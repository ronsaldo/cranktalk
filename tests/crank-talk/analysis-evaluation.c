#include "assertions.h"
#include "crank-talk/context.h"
#include "crank-talk/eval.h"
#include "crank-talk/string.h"

void analysisAndEvaluation_runTests(void)
{
    crank_talk_VMContext_t *context = crank_talk_vm_createContext();

    // Empty string
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "");
        AssertEquals(crank_talk_oop_nil(), value);
    }

    // Literal character
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "$A");
        AssertEquals('A', crank_talk_oop_decodeCharacter(value));
    }

    // Literal integer
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "42");
        AssertEquals(42, crank_talk_oop_decodeSmallInteger(value));
    }

    // Literal float
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "42.5");
        AssertEquals(42.5, crank_talk_oop_asFloat(value));
    }

    // Literal string
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "'Hello World'");
        Assert(crank_talk_String_equalsCString(value, "Hello World"));
    }

    // Literal symbol
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "#testSymbol");
        Assert(crank_talk_Symbol_equalsCString(value, "testSymbol"));
    }

    crank_talk_vm_destroyContext(context);
}