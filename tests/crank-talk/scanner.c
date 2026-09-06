#include "crank-talk/scanner.h"
#include "crank-talk/context.h"
#include "crank-talk/string.h"
#include "assertions.h"

static crank_talk_oop_t
scanString(crank_talk_VMContext_t *context, const char *sourceString)
{
    crank_talk_oop_t sourceCodeValue = CrankAllocateValuesObjectWithClassName(context, SourceCode);
    crank_talk_SourceCode_t *sourceCode = (crank_talk_SourceCode_t *)sourceCodeValue;
    sourceCode->text = (crank_talk_String_t*)crank_talk_String_createWithCString(context, sourceString);
    sourceCode->name = (crank_talk_String_t*)crank_talk_String_createWithCString(context, "<cli>");

    return crank_talk_CrankTalk_scanSourceCode(context, sourceCodeValue);
}

static crank_talk_TokenKind_t
getTokenKind(crank_talk_oop_t tokenValue)
{
    crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t*)tokenValue;
    return crank_talk_oop_decodeSmallInteger(token->kind);
}

void scanner_runTests(void)
{
    crank_talk_VMContext_t *context = crank_talk_vm_createContext();

    // Empty source
    {
        crank_talk_oop_t tokensValue = scanString(context, "");
        AssertEquals(1, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[0]));
    }
    
    // Whitespaces
    {
        crank_talk_oop_t tokensValue = scanString(context, "   ");
        AssertEquals(1, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[0]));
    }

    // Comment
    {
        crank_talk_oop_t tokensValue = scanString(context, "\" Testing \"");
        AssertEquals(1, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[0]));
    }

    // Incomplete comment
    {
        crank_talk_oop_t tokensValue = scanString(context, "\" Testing");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindError, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // Identifier
    {
        crank_talk_oop_t tokensValue = scanString(context, "helloIdentifier");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindIdentifier, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // Keyword
    {
        crank_talk_oop_t tokensValue = scanString(context, "keyword:");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindKeyword, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // Integer
    {
        crank_talk_oop_t tokensValue = scanString(context, "1234");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindInteger, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "-1234");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindInteger, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "2r01010");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindInteger, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "16rC0DE");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindInteger, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    // Float
    {
        crank_talk_oop_t tokensValue = scanString(context, "42.5");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindFloat, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "42.5e+12");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindFloat, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "42.5E+12");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindFloat, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "42.5e-12");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindFloat, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "42.5E-12");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindFloat, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // Symbol
    {
        crank_talk_oop_t tokensValue = scanString(context, "#symbol");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindSymbol, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // KeywordSymbol
    {
        crank_talk_oop_t tokensValue = scanString(context, "#keyword:");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindSymbol, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // Operator symbol
    {
        crank_talk_oop_t tokensValue = scanString(context, "#<");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindSymbol, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "#|");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindSymbol, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "#+");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindSymbol, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // String symbol
    {
        crank_talk_oop_t tokensValue = scanString(context, "#'Hello Symbol'");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindSymbol, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // Incomplete string symbol
    {
        crank_talk_oop_t tokensValue = scanString(context, "#'Hello Symbol");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindError, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // Hash punctuations
    {
        crank_talk_oop_t tokensValue = scanString(context, "#[");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindByteArrayStart, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    {
        crank_talk_oop_t tokensValue = scanString(context, "#(");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindLiteralArrayStart, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    // Method start
    {
        crank_talk_oop_t tokensValue = scanString(context, "![");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindMethodStart, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }
    // String
    {
        crank_talk_oop_t tokensValue = scanString(context, "'Hello Symbol'");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindString, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // Incomplete String
    {
        crank_talk_oop_t tokensValue = scanString(context, "'Hello Symbol");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindError, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    // Character
    {
        crank_talk_oop_t tokensValue = scanString(context, "$a");
        AssertEquals(2, crank_talk_object_getSizeInSlots(tokensValue));

        crank_talk_Array_t *tokens = (crank_talk_Array_t *)tokensValue;
        AssertEquals(CrankTalkTokenKindCharacter, getTokenKind(tokens->elements[0]));
        AssertEquals(CrankTalkTokenKindEndOfSource, getTokenKind(tokens->elements[1]));
    }

    crank_talk_vm_destroyContext(context);
}
