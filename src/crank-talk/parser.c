#include "crank-talk/parser.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/ordered-collection.h"
#include "crank-talk/scanner.h"
#include "crank-talk/source-position.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct crank_talk_ParserState_s
{
    crank_talk_Array_t *tokens;
    size_t position;
    size_t tokenCount;
} crank_talk_ParserState_t;

static bool
crank_talk_ParserState_atEnd(crank_talk_ParserState_t *state)
{
    return state->position >= state->tokenCount;
}

static size_t
crank_talk_ParserState_memento(crank_talk_ParserState_t *state)
{
    return state->position;
}

static void
crank_talk_ParserState_restore(crank_talk_ParserState_t *state, size_t memento)
{
    state->position = memento;
}

static crank_talk_TokenKind_t
crank_talk_ParserState_peekKind(crank_talk_ParserState_t *state, size_t offset)
{
    size_t peekPosition = state->position + offset;
    if(peekPosition < state->tokenCount)
    {
        crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t*)state->tokens->elements[peekPosition];
        return crank_talk_oop_decodeSmallInteger(token->kind);
    }

    return CrankTalkTokenKindEndOfSource;
}

static crank_talk_CrankTalkToken_t*
crank_talk_ParserState_peek(crank_talk_ParserState_t *state, size_t offset)
{
    size_t peekPosition = state->position + offset;
    if(peekPosition < state->tokenCount)
        return (crank_talk_CrankTalkToken_t*)state->tokens->elements[peekPosition];

    return NULL;
}

static void
crank_talk_ParserState_advance(crank_talk_ParserState_t *state)
{
    assert(state->position < state->tokenCount);
    ++state->position;
}

static crank_talk_CrankTalkToken_t*
crank_talk_ParserState_next(crank_talk_ParserState_t *state)
{
    assert(state->position < state->tokenCount);
    return (crank_talk_CrankTalkToken_t*)state->tokens->elements[state->position++];
}

static crank_talk_SourcePosition_t*
crank_talk_ParserState_currentSourcePosition(crank_talk_ParserState_t *state)
{
    if (state->position < state->tokenCount) {
        crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t *)state->tokens->elements[state->position];
        return token->sourcePosition;
    }

    crank_talk_CrankTalkToken_t *lastToken = (crank_talk_CrankTalkToken_t *)state->tokens->elements[state->tokenCount - 1];
    return lastToken->sourcePosition;
}

static crank_talk_SourcePosition_t*
crank_talk_ParserState_previousSourcePosition(crank_talk_ParserState_t *state)
{
    assert(state->position > 0);
    crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t *)state->tokens->elements[state->position - 1];
    return token->sourcePosition;
}

static crank_talk_SourcePosition_t *
crank_talk_ParserState_sourcePositionFrom(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state, size_t startingPosition)
{
    assert(startingPosition < state->tokenCount);
    crank_talk_oop_t startSourcePosition = (crank_talk_oop_t)((crank_talk_CrankTalkToken_t *)state->tokens->elements[startingPosition])->sourcePosition;
    if(state->position > 0)
    {
        crank_talk_oop_t endSourcePosition = (crank_talk_oop_t)crank_talk_ParserState_previousSourcePosition(state);
        return (crank_talk_SourcePosition_t*)crank_talk_SourcePosition_to(context, startSourcePosition, endSourcePosition);
    }
    else
    {
        crank_talk_oop_t endSourcePosition = (crank_talk_oop_t)crank_talk_ParserState_currentSourcePosition(state);
        return (crank_talk_SourcePosition_t*)crank_talk_SourcePosition_until(context, startSourcePosition, endSourcePosition);
    }
}

static crank_talk_oop_t
crank_talk_ParserState_advanceWithExpectedError(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state, const char *errorMessage)
{
    crank_talk_oop_t errorNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeParseErrorNode);
    crank_talk_ParseTreeParseErrorNode_t *errorNode = (crank_talk_ParseTreeParseErrorNode_t *)errorNodeValue;
    if(crank_talk_ParserState_peekKind(state, 0) == CrankTalkTokenKindError)
    {
        crank_talk_CrankTalkToken_t *errorToken = crank_talk_ParserState_next(state);
        errorNode->super.sourcePosition = errorToken->sourcePosition;
        if(errorToken->errorMessage)
            errorNode->errorMessage = (crank_talk_oop_t)errorToken->errorMessage;
        else
            errorNode->errorMessage = crank_talk_String_createWithCString(context, errorMessage);
    }
    else if(crank_talk_ParserState_atEnd(state))
    {
        errorNode->super.sourcePosition = (crank_talk_SourcePosition_t*)crank_talk_ParserState_currentSourcePosition(state);
        errorNode->errorMessage = crank_talk_String_createWithCString(context, errorMessage);
    }
    else
    {
        errorNode->super.sourcePosition = (crank_talk_SourcePosition_t*)crank_talk_ParserState_currentSourcePosition(state);
        errorNode->errorMessage = crank_talk_String_createWithCString(context, errorMessage);
        crank_talk_ParserState_advance(state);
    }

    return errorNodeValue;
}

static crank_talk_oop_t
crank_talk_ParserState_expectAddingErrorToNode(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state, crank_talk_TokenKind_t expectedKind, crank_talk_oop_t nodeValue)
{
    if(crank_talk_ParserState_peekKind(state, 0) == expectedKind)
    {
        crank_talk_ParserState_advance(state);
        return nodeValue;
    }

    char tempBuffer[256];
    snprintf(tempBuffer, sizeof(tempBuffer), "Expected a token of kind %s", crank_talk_TokenKind_toString(expectedKind));

    crank_talk_SourcePosition_t *errorPosition = crank_talk_ParserState_currentSourcePosition(state);
    
    crank_talk_oop_t errorNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeParseErrorNode);
    crank_talk_ParseTreeParseErrorNode_t *errorNode = (crank_talk_ParseTreeParseErrorNode_t*)errorNodeValue;
    errorNode->super.sourcePosition = errorPosition;
    errorNode->errorMessage = crank_talk_String_createWithCString(context, tempBuffer);

    crank_talk_oop_t sequenceNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeSequenceNode);
    crank_talk_ParseTreeSequenceNode_t *sequenceNode = (crank_talk_ParseTreeSequenceNode_t *)sequenceNodeValue;
    sequenceNode->super.sourcePosition = errorPosition;
    sequenceNode->pragmas = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    sequenceNode->localVariables = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    sequenceNode->expressions = (crank_talk_Array_t*)crank_talk_Array_WithWith(context, nodeValue, errorNodeValue);

    return sequenceNodeValue;
}

static int64_t
parseIntegerConstant(size_t constantStringSize, const char *constantString)
{
    // TODO: Support large integers.
    int64_t result = 0;
    int64_t radix = 10;
    bool hasSeenRadix = false;

    for (size_t i = 0; i < constantStringSize; ++i)
    {
        char c = constantString[i];
        if (!hasSeenRadix && (c == 'r' || c == 'R'))
        {
            hasSeenRadix = true;
            radix = result;
            result = 0;
        }
        else
        {
            if ('0' <= c && c <= '9')
                result = result * radix + (int64_t)(c - '0');
            else if ('A' <= c && c <= 'Z')
                result = result * radix + (int64_t)(c - 'A' + 10);
            else if ('a' <= c && c <= 'z')
                result = result * radix + (int64_t)(c - 'a' + 10);
        }
    }
    return result;
}

static crank_talk_oop_t
crank_talk_parseLiteralInteger(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindInteger);

    crank_talk_StringSlice_t tokenString = crank_talk_SourcePosition_asStringSlice(token->sourcePosition);

    crank_talk_oop_t literalNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    crank_talk_ParseTreeLiteralNode_t *literalNode = (crank_talk_ParseTreeLiteralNode_t*)literalNodeValue;
    literalNode->super.sourcePosition = token->sourcePosition;
    literalNode->value = crank_talk_oop_encodeSmallInteger(parseIntegerConstant(tokenString.size, tokenString.text));
    return literalNodeValue;
}

static crank_talk_oop_t
crank_talk_parseLiteralFloat(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindFloat);

    crank_talk_StringSlice_t tokenString = crank_talk_SourcePosition_asStringSlice(token->sourcePosition);
    char *tempString = malloc(tokenString.size + 1);
    memcpy(tempString, tokenString.text, tokenString.size);
    tempString[tokenString.size] = 0;
    double value = atof(tempString);
    free(tempString);

    crank_talk_oop_t literalNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    crank_talk_ParseTreeLiteralNode_t *literalNode = (crank_talk_ParseTreeLiteralNode_t*)literalNodeValue;
    literalNode->super.sourcePosition = token->sourcePosition;
    literalNode->value = crank_talk_oop_fromFloat(context, value);
    return literalNodeValue;
}

static crank_talk_oop_t
crank_talk_parseLiteralCharacter(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindCharacter);

    crank_talk_StringSlice_t tokenString = crank_talk_SourcePosition_asStringSlice(token->sourcePosition);
    char characterValue = tokenString.text[1];

    crank_talk_oop_t literalNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    crank_talk_ParseTreeLiteralNode_t *literalNode = (crank_talk_ParseTreeLiteralNode_t*)literalNodeValue;
    literalNode->super.sourcePosition = token->sourcePosition;
    literalNode->value = crank_talk_oop_encodeCharacter(characterValue);
    return literalNodeValue;
}

static crank_talk_oop_t
crank_talk_parseNilReference(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindNil);

    crank_talk_oop_t pseudoReferenceValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeNilReferenceNode);
    crank_talk_ParseTreeNilReferenceNode_t *pseudoReference = (crank_talk_ParseTreeNilReferenceNode_t*)pseudoReferenceValue;
    pseudoReference->super.super.sourcePosition = token->sourcePosition;
    return pseudoReferenceValue;
}

static crank_talk_oop_t
crank_talk_parseFalseReference(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindFalse);

    crank_talk_oop_t pseudoReferenceValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeFalseReferenceNode);
    crank_talk_ParseTreeFalseReferenceNode_t *pseudoReference = (crank_talk_ParseTreeFalseReferenceNode_t*)pseudoReferenceValue;
    pseudoReference->super.super.sourcePosition = token->sourcePosition;
    return pseudoReferenceValue;
}

static crank_talk_oop_t
crank_talk_parseTrueReference(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindTrue);

    crank_talk_oop_t pseudoReferenceValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeTrueReferenceNode);
    crank_talk_ParseTreeTrueReferenceNode_t *pseudoReference = (crank_talk_ParseTreeTrueReferenceNode_t*)pseudoReferenceValue;
    pseudoReference->super.super.sourcePosition = token->sourcePosition;
    return pseudoReferenceValue;
}

static char *
crank_talk_parseString(size_t stringSize, const char *string, size_t *outSize)
{
    char *result = calloc(1, stringSize + 1);
    char *dest = result;

    for (size_t i = 0; i < stringSize; ++i)
    {
        char c = string[i];
        if (c == '\'')
        {
            char c1 = string[++i];
            if(c1 == '\'')
            {
                *dest++ = '\'';
            }
            else
            {
                break;
            }
        }
        else
        {
            *dest++ = c;
        }
    }

    if (outSize)
        *outSize = dest - result;
    return result;
}

static crank_talk_oop_t
crank_talk_parseLiteralString(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindString);

    crank_talk_StringSlice_t tokenString = crank_talk_SourcePosition_asStringSlice(token->sourcePosition);
    size_t parsedStringSize = 0;
    char *parsedString = crank_talk_parseString(tokenString.size - 2, tokenString.text + 1, &parsedStringSize);
    crank_talk_oop_t stringValue = crank_talk_String_createWithString(context, parsedStringSize, parsedString);
    free(parsedString);

    crank_talk_oop_t literalNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    crank_talk_ParseTreeLiteralNode_t *literalNode = (crank_talk_ParseTreeLiteralNode_t*)literalNodeValue;
    literalNode->super.sourcePosition = token->sourcePosition;
    literalNode->value = stringValue;
    return literalNodeValue;
}

static crank_talk_oop_t
crank_talk_parseLiteral(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    switch(crank_talk_ParserState_peekKind(state, 0))
    {
    case CrankTalkTokenKindInteger:   return crank_talk_parseLiteralInteger(context, state);
    case CrankTalkTokenKindFloat:     return crank_talk_parseLiteralFloat(context, state);
    case CrankTalkTokenKindCharacter: return crank_talk_parseLiteralCharacter(context, state);
    case CrankTalkTokenKindString:    return crank_talk_parseLiteralString(context, state);
    //case CrankTalkTokenKindSymbol:  return crank_talk_parseLiteralSymbol(context, state);
    case CrankTalkTokenKindNil:       return crank_talk_parseNilReference(context, state);
    case CrankTalkTokenKindFalse:     return crank_talk_parseFalseReference(context, state);
    case CrankTalkTokenKindTrue:      return crank_talk_parseTrueReference(context, state);
    default:
        return crank_talk_ParserState_advanceWithExpectedError(context, state, "Expected a literal.");
    }
}

static crank_talk_oop_t
crank_talk_parseSelfReference(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindSelf);

    crank_talk_oop_t pseudoReferenceValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeSelfReferenceNode);
    crank_talk_ParseTreeSelfReferenceNode_t *pseudoReference = (crank_talk_ParseTreeSelfReferenceNode_t*)pseudoReferenceValue;
    pseudoReference->super.super.sourcePosition = token->sourcePosition;
    return pseudoReferenceValue;
}

static crank_talk_oop_t
crank_talk_parseSuperReference(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindSuper);

    crank_talk_oop_t pseudoReferenceValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeSuperReferenceNode);
    crank_talk_ParseTreeSuperReferenceNode_t *pseudoReference = (crank_talk_ParseTreeSuperReferenceNode_t*)pseudoReferenceValue;
    pseudoReference->super.super.sourcePosition = token->sourcePosition;
    return pseudoReferenceValue;
}

static crank_talk_oop_t
crank_talk_parseThisContextReference(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    crank_talk_CrankTalkToken_t *token = crank_talk_ParserState_next(state);
    assert(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindThisContext);

    crank_talk_oop_t pseudoReferenceValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeThisContextReferenceNode);
    crank_talk_ParseTreeThisContextReferenceNode_t *pseudoReference = (crank_talk_ParseTreeThisContextReferenceNode_t*)pseudoReferenceValue;
    pseudoReference->super.super.sourcePosition = token->sourcePosition;
    return pseudoReferenceValue;
}

static crank_talk_oop_t
crank_talk_parseTerm(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    switch(crank_talk_ParserState_peekKind(state, 0))
    {
    case CrankTalkTokenKindSelf:        return crank_talk_parseSelfReference(context, state);
    case CrankTalkTokenKindSuper:       return crank_talk_parseSuperReference(context, state);
    case CrankTalkTokenKindThisContext: return crank_talk_parseThisContextReference(context, state);
    default:
        return crank_talk_parseLiteral(context, state);
    }
}

static crank_talk_oop_t
crank_talk_parseExpression(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    return crank_talk_parseTerm(context, state);
}

static crank_talk_oop_t
crank_talk_parseExpressionListUntilEndOrDelimiter(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state, crank_talk_TokenKind_t delimiter)
{
    crank_talk_oop_t elements = crank_talk_OrderedCollection_create(context);

    // Chop the initial dots
    while(crank_talk_ParserState_peekKind(state, 0) == CrankTalkTokenKindDot)
        crank_talk_ParserState_advance(state);

    // Parse the next expressions
    bool expectsExpression = true;
    while (!crank_talk_ParserState_atEnd(state) && crank_talk_ParserState_peekKind(state, 0) != delimiter)
    {
        if(!expectsExpression)
        {
            crank_talk_oop_t errorNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeParseErrorNode);
            crank_talk_ParseTreeParseErrorNode_t *errorNode = (crank_talk_ParseTreeParseErrorNode_t*)errorNodeValue;

            errorNode->super.sourcePosition = crank_talk_ParserState_currentSourcePosition(state);
            errorNode->errorMessage = crank_talk_String_createWithCString(context, "Expected a dot before the expression.");
            crank_talk_OrderedCollection_add(context, elements, errorNodeValue);
        }

        crank_talk_oop_t expression = crank_talk_parseExpression(context, state);
        crank_talk_OrderedCollection_add(context, elements, expression);

        // Chop the next dot sequence.
        expectsExpression = false;
        while(crank_talk_ParserState_peekKind(state, 0) == CrankTalkTokenKindDot)
        {
            crank_talk_ParserState_advance(state);
            expectsExpression = true;
        }
    }
    
    return crank_talk_OrderedCollection_asArray(context, elements);
}

static crank_talk_oop_t
crank_talk_parseSequenceUntilEndOrDelimiter(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state, crank_talk_TokenKind_t delimiter)
{
    size_t initialPosition = state->position;
    // TODO: Parse the pragmas and the local variables.
    
    crank_talk_oop_t expressions = crank_talk_parseExpressionListUntilEndOrDelimiter(context, state, delimiter);
    if(crank_talk_object_getSizeInSlots(expressions) == 1)
        return crank_talk_oop_getSlotsData(expressions)[0];

    crank_talk_oop_t sequenceValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeSequenceNode);
    crank_talk_ParseTreeSequenceNode_t *sequence = (crank_talk_ParseTreeSequenceNode_t *)sequenceValue;
    sequence->super.sourcePosition = crank_talk_ParserState_sourcePositionFrom(context, state, initialPosition);
    sequence->pragmas = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    sequence->localVariables = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    sequence->expressions = (crank_talk_Array_t*)expressions;
    return sequenceValue;
}

static crank_talk_oop_t
crank_talk_parseTopLevelExpression(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state)
{
    return crank_talk_parseSequenceUntilEndOrDelimiter(context, state, CrankTalkTokenKindEndOfSource);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_parseScannedTokens(crank_talk_VMContext_t *context, crank_talk_oop_t tokensValue)
{
    crank_talk_ParserState_t state = {
        .tokens = (crank_talk_Array_t*)tokensValue,
        .position = 0,
        .tokenCount = crank_talk_object_getSizeInSlots(tokensValue),
    };

    return crank_talk_parseTopLevelExpression(context, &state);
}