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

    // Comment
    {
        crank_talk_oop_t ast = parseString(context, "\"Hello World\"");
        Assert(crank_talk_isParseTreeSequenceNode(context, ast));

        crank_talk_ParseTreeSequenceNode_t *sequenceNode = (crank_talk_ParseTreeSequenceNode_t*)ast;
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->pragmas));
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->localVariables));
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->expressions));
    }

    // Literal integer 42
    {
        crank_talk_oop_t ast = parseString(context, "42");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        AssertEquals(42, crank_talk_oop_decodeSmallInteger(literal->value));
    }

    // Literal integer 10
    {
        crank_talk_oop_t ast = parseString(context, "2r1010");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        AssertEquals(10, crank_talk_oop_decodeSmallInteger(literal->value));
    }

    // Literal integer 49374
    {
        crank_talk_oop_t ast = parseString(context, "16rC0DE");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        AssertEquals(49374, crank_talk_oop_decodeSmallInteger(literal->value));
    }

    // Literal float 42.5
    {
        crank_talk_oop_t ast = parseString(context, "42.5");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        AssertEquals(42.5, crank_talk_oop_asFloat(literal->value));
    }

    // Literal character
    {
        crank_talk_oop_t ast = parseString(context, "$A");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        AssertEquals('A', crank_talk_oop_decodeCharacter(literal->value));
    }

    // nil reference
    {
        crank_talk_oop_t ast = parseString(context, "nil");
        Assert(crank_talk_isParseTreeNilReferenceNode(context, ast));
    }
    
    // false reference
    {
        crank_talk_oop_t ast = parseString(context, "false");
        Assert(crank_talk_isParseTreeFalseReferenceNode(context, ast));
    }

    // true reference
    {
        crank_talk_oop_t ast = parseString(context, "true");
        Assert(crank_talk_isParseTreeTrueReferenceNode(context, ast));
    }

    // self reference
    {
        crank_talk_oop_t ast = parseString(context, "self");
        Assert(crank_talk_isParseTreeSelfReferenceNode(context, ast));
    }

    // super reference
    {
        crank_talk_oop_t ast = parseString(context, "super");
        Assert(crank_talk_isParseTreeSuperReferenceNode(context, ast));
    }

    // thisContext reference
    {
        crank_talk_oop_t ast = parseString(context, "thisContext");
        Assert(crank_talk_isParseTreeThisContextReferenceNode(context, ast));
    }

    crank_talk_vm_destroyContext(context);
}