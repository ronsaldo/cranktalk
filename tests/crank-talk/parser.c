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

    // Assignment
    {
        crank_talk_oop_t ast = parseString(context, "a := 42");
        Assert(crank_talk_isParseTreeAssignmentNode(context, ast));

        crank_talk_ParseTreeAssignmentNode_t *assignment = (crank_talk_ParseTreeAssignmentNode_t*)ast;
        Assert(crank_talk_isParseTreeIdentifierReferenceNode(context, (crank_talk_oop_t)assignment->store));
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)assignment->value));
    }

    // Empty block
    {
        crank_talk_oop_t ast = parseString(context, "[]");
        Assert(crank_talk_isParseTreeBlockNode(context, ast));
        
        crank_talk_ParseTreeBlockNode_t *blockNode = (crank_talk_ParseTreeBlockNode_t*)ast;
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)blockNode->arguments));
        Assert(crank_talk_isParseTreeSequenceNode(context, (crank_talk_oop_t)blockNode->body));
    }
    
    // Block body
    {
        crank_talk_oop_t ast = parseString(context, "[42]");
        Assert(crank_talk_isParseTreeBlockNode(context, ast));
        
        crank_talk_ParseTreeBlockNode_t *blockNode = (crank_talk_ParseTreeBlockNode_t*)ast;
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)blockNode->arguments));
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)blockNode->body));
    }

    // Block single argument
    {
        crank_talk_oop_t ast = parseString(context, "[:a | a]");
        Assert(crank_talk_isParseTreeBlockNode(context, ast));
        
        crank_talk_ParseTreeBlockNode_t *blockNode = (crank_talk_ParseTreeBlockNode_t*)ast;
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)blockNode->arguments));

        Assert(crank_talk_isParseTreeIdentifierReferenceNode(context, (crank_talk_oop_t)blockNode->body));
    }

    // Incomplete block
    {
        parseString(context, "[:a | a");
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

    // Literal string
    {
        crank_talk_oop_t ast = parseString(context, "'Hello World'");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        Assert(crank_talk_String_equalsCString(literal->value, "Hello World"));
    }

    // Literal string 2
    {
        crank_talk_oop_t ast = parseString(context, "'''Hello World'''");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        Assert(crank_talk_String_equalsCString(literal->value, "'Hello World'"));
    }

    // Literal symbol identifier
    {
        crank_talk_oop_t ast = parseString(context, "#symbol");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        Assert(crank_talk_Symbol_equalsCString(literal->value, "symbol"));
    }

    // Literal symbol keyword
    {
        crank_talk_oop_t ast = parseString(context, "#keyword:");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        Assert(crank_talk_Symbol_equalsCString(literal->value, "keyword:"));
    }

    // Literal symbol multi-keyword
    {
        crank_talk_oop_t ast = parseString(context, "#keyword:with:");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        Assert(crank_talk_Symbol_equalsCString(literal->value, "keyword:with:"));
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

    // Identifier reference
    {
        crank_talk_oop_t ast = parseString(context, "identifier");
        Assert(crank_talk_isParseTreeIdentifierReferenceNode(context, ast));

        crank_talk_ParseTreeIdentifierReferenceNode_t *literal = (crank_talk_ParseTreeIdentifierReferenceNode_t*)ast;
        Assert(crank_talk_Symbol_equalsCString((crank_talk_oop_t)literal->symbol, "identifier"));
    }

    // Parenthesis
    {
        crank_talk_oop_t ast = parseString(context, "(42)");
        Assert(crank_talk_isParseTreeLiteralNode(context, ast));

        crank_talk_ParseTreeLiteralNode_t *literal = (crank_talk_ParseTreeLiteralNode_t*)ast;
        AssertEquals(42, crank_talk_oop_decodeSmallInteger(literal->value));
    }

    // Empty literal array
    {
        crank_talk_oop_t ast = parseString(context, "#()");
        Assert(crank_talk_isParseTreeLiteralArrayNode(context, ast));

        crank_talk_ParseTreeLiteralArrayNode_t *literalArray = (crank_talk_ParseTreeLiteralArrayNode_t*)ast;
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)literalArray->elements));
    }

    // Literal array
    {
        crank_talk_oop_t ast = parseString(context, "#(1  2 true false ())");
        Assert(crank_talk_isParseTreeLiteralArrayNode(context, ast));

        crank_talk_ParseTreeLiteralArrayNode_t *literalArray = (crank_talk_ParseTreeLiteralArrayNode_t*)ast;
        AssertEquals(5, crank_talk_object_getSizeInSlots((crank_talk_oop_t)literalArray->elements));
    }

    // Empty array
    {
        crank_talk_oop_t ast = parseString(context, "{}");
        Assert(crank_talk_isParseTreeArrayNode(context, ast));

        crank_talk_ParseTreeArrayNode_t *arrayNode = (crank_talk_ParseTreeArrayNode_t *)ast;
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)arrayNode->elements));
    }

    // Two elements array
    {
        crank_talk_oop_t ast = parseString(context, "{1. 2}");
        Assert(crank_talk_isParseTreeArrayNode(context, ast));

        crank_talk_ParseTreeArrayNode_t *arrayNode = (crank_talk_ParseTreeArrayNode_t *)ast;
        AssertEquals(2, crank_talk_object_getSizeInSlots((crank_talk_oop_t)arrayNode->elements));

        crank_talk_oop_t *arrayElements = arrayNode->elements->elements;
        Assert(crank_talk_isParseTreeLiteralNode(context, arrayElements[0]));
        Assert(crank_talk_isParseTreeLiteralNode(context, arrayElements[1]));
    }

    // Binary expression sequence
    {
        crank_talk_oop_t ast = parseString(context, "1 + 2");
        Assert(crank_talk_isParseTreeMessageSendNode(context, ast));

        crank_talk_ParseTreeMessageSendNode_t *binarySequence = (crank_talk_ParseTreeMessageSendNode_t*)ast;
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)binarySequence->arguments));
        
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)binarySequence->receiver));
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)binarySequence->selector));
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)binarySequence->arguments->elements[0]));        
    }

    // Unary message send
    {
        crank_talk_oop_t ast = parseString(context, "a negated");
        Assert(crank_talk_isParseTreeMessageSendNode(context, ast));

        crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t*)ast;
        Assert(crank_talk_isParseTreeIdentifierReferenceNode(context, (crank_talk_oop_t)sendNode->receiver));
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)sendNode->selector));
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments));
    }

    // Unary message send2
    {
        crank_talk_oop_t ast = parseString(context, "42 negated");
        Assert(crank_talk_isParseTreeMessageSendNode(context, ast));

        crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t*)ast;
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)sendNode->receiver));
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)sendNode->selector));
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments));
    }

    // Keyword message send
    {
        crank_talk_oop_t ast = parseString(context, "a perform: #yourself");
        Assert(crank_talk_isParseTreeMessageSendNode(context, ast));

        crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t*)ast;
        Assert(crank_talk_isParseTreeIdentifierReferenceNode(context, (crank_talk_oop_t)sendNode->receiver));
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)sendNode->selector));
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments));

        crank_talk_Array_t *arguments = sendNode->arguments;
        Assert(crank_talk_isParseTreeLiteralNode(context, arguments->elements[0]));
    }

    // Keyword message send 2
    {
        crank_talk_oop_t ast = parseString(context, "a perform: #yourself with: 42");
        Assert(crank_talk_isParseTreeMessageSendNode(context, ast));

        crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t*)ast;
        Assert(crank_talk_isParseTreeIdentifierReferenceNode(context, (crank_talk_oop_t)sendNode->receiver));
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)sendNode->selector));
        AssertEquals(2, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments));

        crank_talk_Array_t *arguments = sendNode->arguments;
        Assert(crank_talk_isParseTreeLiteralNode(context, arguments->elements[0]));
        Assert(crank_talk_isParseTreeLiteralNode(context, arguments->elements[1]));
    }

    // Cascade message send
    {
        crank_talk_oop_t ast = parseString(context, "a; with:5; yourself");
        Assert(crank_talk_isParseTreeMessageCascadeNode(context, ast));

        crank_talk_ParseTreeMessageCascadeNode_t *cascadeNode = (crank_talk_ParseTreeMessageCascadeNode_t*)ast;
        Assert(crank_talk_isParseTreeIdentifierReferenceNode(context, (crank_talk_oop_t)cascadeNode->receiver));
        AssertEquals(2, crank_talk_object_getSizeInSlots((crank_talk_oop_t)cascadeNode->cascadedMessages));

        crank_talk_Array_t *messages = cascadeNode->cascadedMessages;

        Assert(crank_talk_isParseTreeCascadedMessageNode(context, messages->elements[0]));
        crank_talk_ParseTreeCascadedMessageNode_t *message = (crank_talk_ParseTreeCascadedMessageNode_t *)messages->elements[0];
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)message->selector));
        Assert(crank_talk_Symbol_equalsCString(((crank_talk_ParseTreeLiteralNode_t*)message->selector)->value, "with:"));
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)message->arguments));

        Assert(crank_talk_isParseTreeCascadedMessageNode(context, messages->elements[1]));
        message = (crank_talk_ParseTreeCascadedMessageNode_t *)messages->elements[1];
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)message->selector));
        Assert(crank_talk_Symbol_equalsCString(((crank_talk_ParseTreeLiteralNode_t*)message->selector)->value, "yourself"));
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)message->arguments));
    }

    // Cascade message send 3
    {
        crank_talk_oop_t ast = parseString(context, "a with: 42; - 3; with: 5 with: 5; yourself");
        Assert(crank_talk_isParseTreeMessageCascadeNode(context, ast));

        crank_talk_ParseTreeMessageCascadeNode_t *cascadeNode = (crank_talk_ParseTreeMessageCascadeNode_t*)ast;
        Assert(crank_talk_isParseTreeIdentifierReferenceNode(context, (crank_talk_oop_t)cascadeNode->receiver));
        AssertEquals(4, crank_talk_object_getSizeInSlots((crank_talk_oop_t)cascadeNode->cascadedMessages));

        crank_talk_Array_t *messages = cascadeNode->cascadedMessages;

        Assert(crank_talk_isParseTreeCascadedMessageNode(context, messages->elements[0]));
        crank_talk_ParseTreeCascadedMessageNode_t *message = (crank_talk_ParseTreeCascadedMessageNode_t *)messages->elements[0];
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)message->selector));
        Assert(crank_talk_Symbol_equalsCString(((crank_talk_ParseTreeLiteralNode_t*)message->selector)->value, "with:"));
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)message->arguments));
        
        Assert(crank_talk_isParseTreeCascadedMessageNode(context, messages->elements[1]));
        message = (crank_talk_ParseTreeCascadedMessageNode_t *)messages->elements[1];
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)message->selector));
        Assert(crank_talk_Symbol_equalsCString(((crank_talk_ParseTreeLiteralNode_t*)message->selector)->value, "-"));
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)message->arguments));

        Assert(crank_talk_isParseTreeCascadedMessageNode(context, messages->elements[2]));
        message = (crank_talk_ParseTreeCascadedMessageNode_t *)messages->elements[2];
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)message->selector));
        Assert(crank_talk_Symbol_equalsCString(((crank_talk_ParseTreeLiteralNode_t*)message->selector)->value, "with:with:"));
        AssertEquals(2, crank_talk_object_getSizeInSlots((crank_talk_oop_t)message->arguments));

        Assert(crank_talk_isParseTreeCascadedMessageNode(context, messages->elements[3]));
        message = (crank_talk_ParseTreeCascadedMessageNode_t *)messages->elements[3];
        Assert(crank_talk_isParseTreeLiteralNode(context, (crank_talk_oop_t)message->selector));
        Assert(crank_talk_Symbol_equalsCString(((crank_talk_ParseTreeLiteralNode_t*)message->selector)->value, "yourself"));
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)message->arguments));
    }

    // Return node
    {
        crank_talk_oop_t ast = parseString(context, "^ self");
        Assert(crank_talk_isParseTreeReturnNode(context, ast));

        crank_talk_ParseTreeReturnNode_t *returnNode = (crank_talk_ParseTreeReturnNode_t*)ast;
        Assert(crank_talk_isParseTreeSelfReferenceNode(context, (crank_talk_oop_t)returnNode->expression));
    }

    // Unary method
    {
        crank_talk_oop_t ast = parseString(context, "![unary ^ self]");
        Assert(crank_talk_isParseTreeMethodDefinitionNode(context, ast));

        crank_talk_ParseTreeMethodDefinitionNode_t *methodNode = (crank_talk_ParseTreeMethodDefinitionNode_t*)ast;
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)methodNode->arguments));

        Assert(crank_talk_Symbol_equalsCString((crank_talk_oop_t)methodNode->selector, "unary"));
        Assert(crank_talk_isParseTreeReturnNode(context, (crank_talk_oop_t)methodNode->body));
    }

    // Binary method
    {
        crank_talk_oop_t ast = parseString(context, "![+ operand ^ self]");
        Assert(crank_talk_isParseTreeMethodDefinitionNode(context, ast));

        crank_talk_ParseTreeMethodDefinitionNode_t *methodNode = (crank_talk_ParseTreeMethodDefinitionNode_t*)ast;
        AssertEquals(2, crank_talk_object_getSizeInSlots((crank_talk_oop_t)methodNode->arguments));

        Assert(crank_talk_Symbol_equalsCString((crank_talk_oop_t)methodNode->selector, "+"));
        Assert(crank_talk_isParseTreeReturnNode(context, (crank_talk_oop_t)methodNode->body));
    }

    // Keyword method
    {
        crank_talk_oop_t ast = parseString(context, "![test: first with: second ^ self]");
        Assert(crank_talk_isParseTreeMethodDefinitionNode(context, ast));

        crank_talk_ParseTreeMethodDefinitionNode_t *methodNode = (crank_talk_ParseTreeMethodDefinitionNode_t*)ast;
        AssertEquals(3, crank_talk_object_getSizeInSlots((crank_talk_oop_t)methodNode->arguments));

        Assert(crank_talk_Symbol_equalsCString((crank_talk_oop_t)methodNode->selector, "test:with:"));
        Assert(crank_talk_isParseTreeReturnNode(context, (crank_talk_oop_t)methodNode->body));
    }

    // Add method
    {
        crank_talk_oop_t ast = parseString(context, "Object ![myYourself ^ self]");
        Assert(crank_talk_isParseTreeAddMethodNode(context, ast));

        crank_talk_ParseTreeAddMethodNode_t *addMethod = (crank_talk_ParseTreeAddMethodNode_t *)ast;

        Assert(crank_talk_isParseTreeMethodDefinitionNode(context, (crank_talk_oop_t)addMethod->method));

        crank_talk_ParseTreeMethodDefinitionNode_t *methodNode = (crank_talk_ParseTreeMethodDefinitionNode_t*)addMethod->method;
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)methodNode->arguments));
        Assert(crank_talk_Symbol_equalsCString((crank_talk_oop_t)methodNode->selector, "myYourself"));
    }

    // Pragmas
    {
        crank_talk_oop_t ast = parseString(context, "<testPragma>");
        Assert(crank_talk_isParseTreeSequenceNode(context, ast));

        crank_talk_ParseTreeSequenceNode_t *sequenceNode = (crank_talk_ParseTreeSequenceNode_t*)ast;
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->pragmas));

        Assert(crank_talk_isParseTreePragmaNode(context, sequenceNode->pragmas->elements[0]));
        crank_talk_ParseTreePragmaNode_t *pragmaNode = (crank_talk_ParseTreePragmaNode_t*)sequenceNode->pragmas->elements[0];
        Assert(crank_talk_Symbol_equalsCString((crank_talk_oop_t)pragmaNode->selector, "testPragma"));
        AssertEquals(0, crank_talk_object_getSizeInSlots((crank_talk_oop_t)pragmaNode->arguments));
    }

    // Pragmas
    {
        crank_talk_oop_t ast = parseString(context, "<test: 1 with: 42>"); 
        Assert(crank_talk_isParseTreeSequenceNode(context, ast));

        crank_talk_ParseTreeSequenceNode_t *sequenceNode = (crank_talk_ParseTreeSequenceNode_t*)ast;
        AssertEquals(1, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->pragmas));

        Assert(crank_talk_isParseTreePragmaNode(context, sequenceNode->pragmas->elements[0]));
        crank_talk_ParseTreePragmaNode_t *pragmaNode = (crank_talk_ParseTreePragmaNode_t*)sequenceNode->pragmas->elements[0];
        Assert(crank_talk_Symbol_equalsCString((crank_talk_oop_t)pragmaNode->selector, "test:with:"));
        AssertEquals(2, crank_talk_object_getSizeInSlots((crank_talk_oop_t)pragmaNode->arguments));
    }

    // Local variables
    {
        crank_talk_oop_t ast = parseString(context, "| local1 local2 |");
        Assert(crank_talk_isParseTreeSequenceNode(context, ast));

        crank_talk_ParseTreeSequenceNode_t *sequenceNode = (crank_talk_ParseTreeSequenceNode_t*)ast;
        AssertEquals(2, crank_talk_object_getSizeInSlots((crank_talk_oop_t)sequenceNode->localVariables));

        crank_talk_oop_t local1Value = sequenceNode->localVariables->elements[0];
        Assert(crank_talk_isParseTreeVariableDefinitionNode(context, local1Value));

        crank_talk_ParseTreeVariableDefinitionNode_t *local1 = (crank_talk_ParseTreeVariableDefinitionNode_t *)local1Value;
        Assert(crank_talk_Symbol_equalsCString((crank_talk_oop_t)local1->name, "local1"));

        crank_talk_oop_t local2Value = sequenceNode->localVariables->elements[1];
        Assert(crank_talk_isParseTreeVariableDefinitionNode(context, local2Value));

        crank_talk_ParseTreeVariableDefinitionNode_t *local2 = (crank_talk_ParseTreeVariableDefinitionNode_t *)local2Value;
        Assert(crank_talk_Symbol_equalsCString((crank_talk_oop_t)local2->name, "local2"));
    }

    crank_talk_vm_destroyContext(context);
}