#include "crank-talk/parser.h"
#include "crank-talk/scanner.h"
#include "crank-talk/context.h"
#include "crank-talk/string.h"
#include "assertions.h"

static crank_talk_oop_t
parseString(crank_talk_VMContext_t *context, const char *sourceString)
{
    crank_talk_oop_t sourceCodeValue = CrankAllocateValuesObjectWithClassName(context, SourceCode);
    crank_talk_SourceCode_t *sourceCode = (crank_talk_SourceCode_t *)sourceCodeValue;
    sourceCode->text = (crank_talk_String_t*)crank_talk_String_createWithCString(context, sourceString);
    sourceCode->name = (crank_talk_String_t*)crank_talk_String_createWithCString(context, "<cli>");

    crank_talk_oop_t tokensValue = crank_talk_CrankTalk_scanSourceCode(context, sourceCodeValue);
    return crank_talk_parseScannedTokens(context, tokensValue);
}

void parser_runTests(void)
{
    crank_talk_VMContext_t *context = crank_talk_vm_createContext();

    // Empty string
    {
        crank_talk_oop_t ast = parseString(context, "");
        Assert(crank_talk_isParseTreeSequenceNode(context, ast));

        crank_talk_ParseTreeSequenceNode_t *sequenceNode = (crank_talk_ParseTreeSequenceNode_t*)ast;
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->pragmas));
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->localVariables));
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->expressions));
    }

    crank_talk_vm_destroyContext(context);
}