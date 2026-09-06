#include "crank-talk/parser.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/scanner.h"
#include "crank-talk/source-position.h"
#include "crank-talk/string.h"
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

static crank_talk_oop_t
crank_talk_ParserState_currentSourcePosition(crank_talk_ParserState_t *state)
{
    if (state->position < state->tokenCount) {
        crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t *)state->tokens->elements[state->position];
        return (crank_talk_oop_t)token->sourcePosition;
    }

    crank_talk_CrankTalkToken_t *lastToken = (crank_talk_CrankTalkToken_t *)state->tokens->elements[state->tokenCount - 1];
    return (crank_talk_oop_t)lastToken->sourcePosition;
}

static crank_talk_oop_t
crank_talk_ParserState_previousSourcePosition(crank_talk_ParserState_t *state)
{
    assert(state->position > 0);
    crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t *)state->tokens->elements[state->position - 1];
    return (crank_talk_oop_t)token->sourcePosition;
}

static crank_talk_oop_t
crank_talk_ParserState_sourcePositionFrom(crank_talk_VMContext_t *context, crank_talk_ParserState_t *state, size_t startingPosition)
{
    assert(startingPosition < state->tokenCount);
    crank_talk_oop_t startSourcePosition = (crank_talk_oop_t)((crank_talk_CrankTalkToken_t *)state->tokens->elements[startingPosition])->sourcePosition;
    if(state->position > 0)
    {
        crank_talk_oop_t endSourcePosition = crank_talk_ParserState_previousSourcePosition(state);
        return crank_talk_SourcePosition_to(context, startSourcePosition, endSourcePosition);
    }
    else
    {
        crank_talk_oop_t endSourcePosition = crank_talk_ParserState_currentSourcePosition(state);
        return crank_talk_SourcePosition_until(context, startSourcePosition, endSourcePosition);
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

    crank_talk_oop_t errorPosition = crank_talk_ParserState_currentSourcePosition(state);
    
    crank_talk_oop_t errorNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeParseErrorNode);
    crank_talk_ParseTreeParseErrorNode_t *errorNode = (crank_talk_ParseTreeParseErrorNode_t*)errorNodeValue;
    errorNode->super.sourcePosition = (crank_talk_SourcePosition_t*)errorPosition;
    errorNode->errorMessage = crank_talk_String_createWithCString(context, tempBuffer);

    crank_talk_oop_t sequenceNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeSequenceNode);
    crank_talk_ParseTreeSequenceNode_t *sequenceNode = (crank_talk_ParseTreeSequenceNode_t *)sequenceNodeValue;
    sequenceNode->super.sourcePosition = (crank_talk_SourcePosition_t*)errorPosition;
    sequenceNode->pragmas = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    sequenceNode->localVariables = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    sequenceNode->expressions = (crank_talk_Array_t*)crank_talk_Array_WithWith(context, nodeValue, errorNodeValue);

    return sequenceNodeValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_parseScannedTokens(crank_talk_VMContext_t *context, crank_talk_oop_t tokensValue)
{
    return crank_talk_oop_nil();
}