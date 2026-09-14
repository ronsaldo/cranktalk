#include "assertions.h"
#include "crank-talk/context.h"
#include "crank-talk/eval.h"
#include "crank-talk/function.h"
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

    // nil reference
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "nil");
        Assert(crank_talk_oop_isNil(value));
    }

    // false reference
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "false");
        AssertEquals(context->roots.falseValue, value);
    }

    // true reference
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "true");
        AssertEquals(context->roots.trueValue, value);
    }

    // Object reference
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "Object");
        AssertEquals(context->roots.ObjectClass, value);
    }

    // Local variable definition
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| local | local");
        AssertEquals(crank_talk_oop_nil(), value);
    }

    // Local variable assignment
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| local | local := 2");
        AssertEquals(2, crank_talk_oop_decodeSmallInteger(value));
    }
    // Local variable assignment
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| local | local := 2. local");
        AssertEquals(2, crank_talk_oop_decodeSmallInteger(value));
    }

    // Empty array
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "{}");
        AssertEquals(0, crank_talk_object_getSizeInSlots(value));
    }
    // Array with two elements
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "{1 . 2}");
        AssertEquals(2, crank_talk_object_getSizeInSlots(value));
        
        crank_talk_oop_t *elements = crank_talk_oop_getSlotsData(value);
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(elements[0]));
        AssertEquals(2, crank_talk_oop_decodeSmallInteger(elements[1]));
    }

    // Unary message
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "42 negated");
        AssertEquals(-42, crank_talk_oop_decodeSmallInteger(value));
    }

    // Message send cascade
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "1 negated; yourself");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));
    }

    // Empty Block
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[] value");
        AssertEquals(crank_talk_oop_nil(), value);
    }

    // Block with literal
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[1] value");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));

        value = crank_talk_analyzeAndEvaluateString(context, "[nil] value");
        AssertEquals(crank_talk_oop_nil(), value);

        value = crank_talk_analyzeAndEvaluateString(context, "[false] value");
        AssertEquals(context->roots.falseValue, value);

        value = crank_talk_analyzeAndEvaluateString(context, "[true] value");
        AssertEquals(context->roots.trueValue, value);
    }

    // Block with argument
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[:x | x] value: 42");
        AssertEquals(42, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with local variables
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[|local| local := 42. local] value");
        AssertEquals(42, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block loading a literal variable
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[Object] value");
        AssertEquals(context->roots.ObjectClass, value);
    }

    // Block message send
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[:x | x negated ] value: 42");
        AssertEquals(-42, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block message send cascade
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[:x | x negated; yourself ] value: 42");
        AssertEquals(42, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with yourself:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[:x | x yourself] value: 42");
        AssertEquals(42, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with class
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[:x | x class] value: 42");
        AssertEquals(context->roots.SmallIntegerClass, value);
    }

    // not
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "true not");
        AssertEquals(context->roots.falseValue, value);

        value = crank_talk_analyzeAndEvaluateString(context, "false not");
        AssertEquals(context->roots.trueValue, value);
    }

    // ifTrue:ifFalse:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "true ifTrue: [1] ifFalse: [2]");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));

        value = crank_talk_analyzeAndEvaluateString(context, "false ifTrue: [1] ifFalse: [2]");
        AssertEquals(2, crank_talk_oop_decodeSmallInteger(value));
    }
    
    // ifFalse:ifTrue:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "true ifFalse: [1] ifTrue: [2]");
        AssertEquals(2, crank_talk_oop_decodeSmallInteger(value));

        value = crank_talk_analyzeAndEvaluateString(context, "false ifFalse: [1] ifTrue: [2]");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));
    }

    // ifTrue:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "true ifTrue: [1]");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));

        value = crank_talk_analyzeAndEvaluateString(context, "false ifTrue: [1]");
        AssertEquals(crank_talk_oop_nil(), value);
    }

    // ifFalse:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "true ifFalse: [1]");
        AssertEquals(crank_talk_oop_nil(), value);

        value = crank_talk_analyzeAndEvaluateString(context, "false ifFalse: [1]");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));
    }

    // whileTrue
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| count | count := 1. [count := count + 1. count <= 10] whileTrue. count");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // whileFalse
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| count | count := 1. [count := count + 1. count > 10] whileFalse. count");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // whileTrue:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| count | count := 1. [count <= 10] whileTrue: [count := count + 1]. count");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // whileFalse:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| count | count := 1. [count > 10] whileFalse: [count := count + 1]. count");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // whileNil:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| count | [ count ] whileNil: [ count := 42 ]. count");
        AssertEquals(42, crank_talk_oop_decodeSmallInteger(value));
    }
    // whileNotNil:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| count | count := 42. [ count ] whileNotNil: [ count := nil ]. count");
        AssertEquals(crank_talk_oop_nil(), value);
    }

    // doWhileTrue:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| count | count := 1. [count := count + 1] doWhileTrue: [count <= 10]. count");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // doWhileFalse:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "| count | count := 1. [count := count + 1] doWhileFalse: [count > 10]. count");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with ifTrue:ifFalse:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[:x | x ifTrue: [1] ifFalse: [2]] value: true");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));

        value = crank_talk_analyzeAndEvaluateString(context, "[:x | x ifTrue: [1] ifFalse: [2]] value: false");
        AssertEquals(2, crank_talk_oop_decodeSmallInteger(value));
    }
    
    // Block with ifFalse:ifTrue:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[:x | x ifFalse: [1] ifTrue: [2]] value: true");
        AssertEquals(2, crank_talk_oop_decodeSmallInteger(value));

        value = crank_talk_analyzeAndEvaluateString(context, "[:x | x ifFalse: [1] ifTrue: [2]] value: false");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with ifTrue:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[:x | x ifTrue: [1]] value: true");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));

        value = crank_talk_analyzeAndEvaluateString(context, "[:x | x ifTrue: [1]] value: false");
        AssertEquals(crank_talk_oop_nil(), value);
    }

    // Block with ifFalse:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[:x | x ifFalse: [1]] value: true");
        AssertEquals(crank_talk_oop_nil(), value);

        value = crank_talk_analyzeAndEvaluateString(context, "[:x | x ifFalse: [1]] value: false");
        AssertEquals(1, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with whileTrue
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[| count | count := 1. [count := count + 1. count <= 10] whileTrue. count] value");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with whileTrue:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[| count | count := 1. [count <= 10] whileTrue: [count := count + 1]. count] value");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with whileFalse
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[| count | count := 1. [count := count + 1. count > 10] whileFalse. count] value");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with whileFalse:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[| count | count := 1. [count > 10] whileFalse: [count := count + 1]. count] value");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }
 
    // Block with whileNil:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[| count | [ count ] whileNil: [ count := 42 ]. count] value");
        AssertEquals(42, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with whileNotNil:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[| count | count := 42. [ count ] whileNotNil: [ count := nil ]. count] value");
        AssertEquals(crank_talk_oop_nil(), value);
    }

    // Block with doWhileTrue:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[| count | count := 1. [count := count + 1] doWhileTrue: [count <= 10]. count] value");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with doWhileFalse:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "[| count | count := 1. [count := count + 1] doWhileFalse: [count > 10]. count] value");
        AssertEquals(11, crank_talk_oop_decodeSmallInteger(value));
    }

    // Block with block:
    {
        crank_talk_oop_t value = crank_talk_analyzeAndEvaluateString(context, "([:x | [:y | x + y]] value: 42) value: 5");
        AssertEquals(47, crank_talk_oop_decodeSmallInteger(value));
    }
    crank_talk_vm_destroyContext(context);
}