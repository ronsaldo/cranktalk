#include "crank-talk/scanner.h"
#include "crank-talk/context.h"
#include "crank-talk/ordered-collection.h"
#include "crank-talk/string.h"
#include <stdlib.h>

static const char *CrankTalkTokenToStringTable[] = {
#define TokenKind(name) #name,
#include "crank-talk/token-kind.inc"
#undef TokenKind
};

CRANK_TALK_API const char *
crank_talk_TokenKind_toString(crank_talk_TokenKind_t tokenKind)
{
    return CrankTalkTokenToStringTable[tokenKind];
}

typedef struct crank_talk_ScannerState_s
{
    crank_talk_SourceCode_t *sourceCode;
    const char *text;
    size_t textSize;
    size_t position;
    size_t line;
    size_t column;
    bool isPreviousCR;
} crank_talk_ScannerState_t;

static bool
crank_talk_ScannerState_atEnd(crank_talk_ScannerState_t* state)
{
    return state->position >= state->textSize;
}

static int
crank_talk_ScannerState_peekAt(crank_talk_ScannerState_t* state, size_t offset)
{
    size_t peekPosition = state->position + offset;
    if(peekPosition < state->textSize)
        return state->text[peekPosition];
    else
        return -1;
}

static void
crank_talk_ScannerState_advance(crank_talk_ScannerState_t* state)
{
    assert(state->position < state->textSize);

    char c = state->text[state->position];
    ++state->position;

    if(c == '\r')
    {
        ++state->line;
        state->column = 1;
        state->isPreviousCR = true;
    }
    else if(c == '\n')
    {
        if(!state->isPreviousCR)
        {
            ++state->line;
            state->column = 1;
        }

        state->isPreviousCR = false;
    }
    else if(c == '\t')
    {
        // TODO: Check the correctnes of this
        state->column = (state->column + 3) / 4 * 4 + 1;
        state->isPreviousCR = false;
    }
    else
    {
        ++state->column;
        state->isPreviousCR = false;
    }

}

static void
crank_talk_ScannerState_advanceCount(crank_talk_ScannerState_t* state, size_t count)
{
    for(size_t i = 0; i < count; ++i)
        crank_talk_ScannerState_advance(state);
}

static crank_talk_oop_t
crank_talk_ScannerState_makeToken(crank_talk_VMContext_t *context, crank_talk_ScannerState_t* state, crank_talk_TokenKind_t kind)
{
    crank_talk_oop_t sourcePositionValue = CrankAllocateValuesObjectWithClassName(context, SourcePosition);
    crank_talk_SourcePosition_t *sourcePosition = (crank_talk_SourcePosition_t*)sourcePositionValue;
    sourcePosition->sourceCode  = state->sourceCode;
    sourcePosition->startIndex  = crank_talk_oop_encodeSmallInteger(state->position);
    sourcePosition->endIndex    = crank_talk_oop_encodeSmallInteger(state->position);
    sourcePosition->startLine   = crank_talk_oop_encodeSmallInteger(state->line);
    sourcePosition->endLine     = crank_talk_oop_encodeSmallInteger(state->line);
    sourcePosition->startColumn = crank_talk_oop_encodeSmallInteger(state->column);
    sourcePosition->endColumn   = crank_talk_oop_encodeSmallInteger(state->column);

    crank_talk_oop_t tokenValue = CrankAllocateValuesObjectWithClassName(context, CrankTalkToken);
    crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t*)tokenValue;
    token->sourcePosition = sourcePosition;
    token->kind = crank_talk_oop_encodeSmallInteger(kind);
    return tokenValue;
}

static crank_talk_oop_t
crank_talk_ScannerState_makeTokenStartingFrom(crank_talk_VMContext_t *context, crank_talk_ScannerState_t* state, crank_talk_TokenKind_t kind, crank_talk_ScannerState_t* initialState)
{
    crank_talk_oop_t sourcePositionValue = CrankAllocateValuesObjectWithClassName(context, SourcePosition);
    crank_talk_SourcePosition_t *sourcePosition = (crank_talk_SourcePosition_t*)sourcePositionValue;
    sourcePosition->sourceCode  = state->sourceCode;
    sourcePosition->startIndex  = crank_talk_oop_encodeSmallInteger(initialState->position);
    sourcePosition->endIndex    = crank_talk_oop_encodeSmallInteger(state->position);
    sourcePosition->startLine   = crank_talk_oop_encodeSmallInteger(initialState->line);
    sourcePosition->endLine     = crank_talk_oop_encodeSmallInteger(state->line);
    sourcePosition->startColumn = crank_talk_oop_encodeSmallInteger(initialState->column);
    sourcePosition->endColumn   = crank_talk_oop_encodeSmallInteger(state->column);

    crank_talk_oop_t tokenValue = CrankAllocateValuesObjectWithClassName(context, CrankTalkToken);
    crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t*)tokenValue;
    token->sourcePosition = sourcePosition;
    token->kind = crank_talk_oop_encodeSmallInteger(kind);
    return tokenValue;
}

static crank_talk_oop_t
crank_talk_ScannerState_makeErrorTokenStartingFrom(crank_talk_VMContext_t *context, crank_talk_ScannerState_t* state, const char *errorMessage, crank_talk_ScannerState_t* initialState)
{
    crank_talk_oop_t sourcePositionValue = CrankAllocateValuesObjectWithClassName(context, SourcePosition);
    crank_talk_SourcePosition_t *sourcePosition = (crank_talk_SourcePosition_t*)sourcePositionValue;
    sourcePosition->sourceCode  = state->sourceCode;
    sourcePosition->startIndex  = crank_talk_oop_encodeSmallInteger(initialState->position);
    sourcePosition->endIndex    = crank_talk_oop_encodeSmallInteger(state->position);
    sourcePosition->startLine   = crank_talk_oop_encodeSmallInteger(initialState->line);
    sourcePosition->endLine     = crank_talk_oop_encodeSmallInteger(state->line);
    sourcePosition->startColumn = crank_talk_oop_encodeSmallInteger(initialState->column);
    sourcePosition->endColumn   = crank_talk_oop_encodeSmallInteger(state->column);

    crank_talk_oop_t tokenValue = CrankAllocateValuesObjectWithClassName(context, CrankTalkToken);
    crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t*)tokenValue;
    token->sourcePosition = sourcePosition;
    token->errorMessage = (crank_talk_String_t*)crank_talk_String_createWithCString(context, errorMessage);
    token->kind = crank_talk_oop_encodeSmallInteger(CrankTalkTokenKindError);
    return tokenValue;
}

static bool
isDigit(char c)
{
    return '0' <= c && c <= '9';
}

static bool
isIdentifierStart(char c)
{
    return
        ('A' <= c && c <= 'Z') ||
        ('a' <= c && c <= 'z') ||
        '_' == c;
}

static bool
isIdentifierMiddle(char c)
{
    return isIdentifierStart(c) || isDigit(c);
}

static bool isOperatorCharacter(char c)
{
    const char *operatorSet = "+-/\\*~<>=@,%|&?!";
    for(const char *position = operatorSet; *position; ++position)
    {
        if(*position == c)
            return true;
    }

    return false;
}

static crank_talk_oop_t
crank_talk_ScannerState_skipWhite(crank_talk_VMContext_t *context, crank_talk_ScannerState_t *state)
{
    bool hasSeenComment = true;
    while (hasSeenComment)
    {
        hasSeenComment = false;

        // Skip the white spaces.
        while(!crank_talk_ScannerState_atEnd(state) && crank_talk_ScannerState_peekAt(state, 0) <= ' ')
            crank_talk_ScannerState_advance(state);

        // Multi-line comment
        if(crank_talk_ScannerState_peekAt(state, 0) == '"')
        {
            crank_talk_ScannerState_t commentInitialState = *state;
            crank_talk_ScannerState_advance(state);
            while(!crank_talk_ScannerState_atEnd(state) && crank_talk_ScannerState_peekAt(state, 0) != '"')
                crank_talk_ScannerState_advance(state);

            if(crank_talk_ScannerState_peekAt(state, 0) == '"')
                crank_talk_ScannerState_advance(state);
            else
                return crank_talk_ScannerState_makeErrorTokenStartingFrom(context, state, "Incomplete comment.", &commentInitialState);

            hasSeenComment = true;
        }
    }

    return crank_talk_oop_nil();
}

static bool
crank_talk_ScannerState_scanAdvanceKeyword(crank_talk_ScannerState_t *state)
{
    if(!isIdentifierStart(crank_talk_ScannerState_peekAt(state, 0)))
        return false;
    
    crank_talk_ScannerState_t initialState = *state;
    while(isIdentifierMiddle(crank_talk_ScannerState_peekAt(state, 0)))
        crank_talk_ScannerState_advance(state);

    if(crank_talk_ScannerState_peekAt(state, 0) == ':')
    {
        crank_talk_ScannerState_advance(state);
        return true;
    }
    else
    {
        *state = initialState;
        return false;
    }
}

static crank_talk_oop_t
crank_talk_ScannerState_scanNextToken(crank_talk_VMContext_t *context, crank_talk_ScannerState_t *state)
{
    crank_talk_oop_t whiteErrorToken = crank_talk_ScannerState_skipWhite(context, state);
    if(!crank_talk_oop_isNil(whiteErrorToken))
        return whiteErrorToken;

    if(crank_talk_ScannerState_atEnd(state))
        return crank_talk_ScannerState_makeToken(context, state, CrankTalkTokenKindEndOfSource);

    // Initial state and peek the first char.
    crank_talk_ScannerState_t initialState = *state;
    int c = crank_talk_ScannerState_peekAt(state, 0);

    // Identifier and keyword
    if(isIdentifierStart(c))
    {
        crank_talk_ScannerState_advance(state);
        while(isIdentifierMiddle(crank_talk_ScannerState_peekAt(state, 0)))
            crank_talk_ScannerState_advance(state);

        if(crank_talk_ScannerState_peekAt(state, 0) == ':')
        {
            crank_talk_ScannerState_advance(state);
            return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindKeyword, &initialState);
        }
        else
        {
            return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindIdentifier, &initialState);
        }
    }

    // Numbers
    if(isDigit(c) || (c == '-' && isDigit(crank_talk_ScannerState_peekAt(state, 1))))
    {
        crank_talk_ScannerState_advance(state);

        while(isDigit(crank_talk_ScannerState_peekAt(state, 0)))
            crank_talk_ScannerState_advance(state);

        // Parse the radix
        if(crank_talk_ScannerState_peekAt(state, 0) == 'R' || crank_talk_ScannerState_peekAt(state, 0) == 'r')
        {
            crank_talk_ScannerState_advance(state);
            while(isIdentifierMiddle(crank_talk_ScannerState_peekAt(state, 0)))
                crank_talk_ScannerState_advance(state);
            return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindInteger, &initialState);
        }

        // Decimal point
        if(crank_talk_ScannerState_peekAt(state, 0) == '.' && isDigit(crank_talk_ScannerState_peekAt(state, 1)))
        {
            crank_talk_ScannerState_advanceCount(state, 2);
            while(isDigit(crank_talk_ScannerState_peekAt(state, 0)))
                crank_talk_ScannerState_advance(state);

            if(crank_talk_ScannerState_peekAt(state, 0) == 'E' || crank_talk_ScannerState_peekAt(state, 0) == 'e')
            {
                int floatExponentOrDigit = crank_talk_ScannerState_peekAt(state, 1);
                if(isDigit(floatExponentOrDigit) ||
                    ((floatExponentOrDigit == '+' || floatExponentOrDigit == '-') && isDigit(crank_talk_ScannerState_peekAt(state, 2))))
                {
                    crank_talk_ScannerState_advanceCount(state, 2);
                    while(isDigit(crank_talk_ScannerState_peekAt(state, 0)))
                        crank_talk_ScannerState_advance(state);
                }
            }

            return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindFloat, &initialState);
        }

        return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindInteger, &initialState);
    }

    if(c == '#')
    {
        int c1 = crank_talk_ScannerState_peekAt(state, 1);
        if(isIdentifierStart(c1))
        {
            crank_talk_ScannerState_advanceCount(state, 2);
            while(isIdentifierMiddle(crank_talk_ScannerState_peekAt(state, 0)))
                crank_talk_ScannerState_advance(state);

            if(crank_talk_ScannerState_peekAt(state, 0) == ':')
            {
                crank_talk_ScannerState_advance(state);
                bool hasAdvanced = true;
                while(hasAdvanced)
                    hasAdvanced = crank_talk_ScannerState_scanAdvanceKeyword(state);
            }

            return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindSymbol, &initialState);
        }
        else if (isOperatorCharacter(c1))
        {
            crank_talk_ScannerState_advanceCount(state, 2);
            while(isIdentifierMiddle(crank_talk_ScannerState_peekAt(state, 0)))
                crank_talk_ScannerState_advance(state);

            return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindSymbol, &initialState);
        }
        else if(c1 == '\'')
        {
            crank_talk_ScannerState_advanceCount(state, 2);
            while(!crank_talk_ScannerState_atEnd(state) && crank_talk_ScannerState_peekAt(state, 0) != '\'')
            {
                if(crank_talk_ScannerState_peekAt(state, 0) == '\'' && crank_talk_ScannerState_peekAt(state, 1) == '\'')
                    crank_talk_ScannerState_advanceCount(state, 2);
                else
                    crank_talk_ScannerState_advance(state);
            }

            // End of string
            if(crank_talk_ScannerState_peekAt(state, 0) != '\'')
                return crank_talk_ScannerState_makeErrorTokenStartingFrom(context, state, "Incomplete symbol string literal.", &initialState);
            crank_talk_ScannerState_advance(state);
            return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindSymbol, &initialState);
        }
        else if(c1 == '[')
        {
            crank_talk_ScannerState_advanceCount(state, 2);
            return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindByteArrayStart, &initialState);
        }
        else if(c1 == '(')
        {
            crank_talk_ScannerState_advanceCount(state, 2);
            return crank_talk_ScannerState_makeTokenStartingFrom(context, state, CrankTalkTokenKindLiteralArrayStart, &initialState);
        }
    }

    // Unexpected character.
    crank_talk_ScannerState_advance(state);
    return crank_talk_ScannerState_makeErrorTokenStartingFrom(context, state, "Unexpected character.", &initialState);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_CrankTalk_scanSourceCode(crank_talk_VMContext_t *context, crank_talk_oop_t sourceCodeValue)
{
    crank_talk_oop_t orderedCollection = crank_talk_OrderedCollection_create(context);
    crank_talk_SourceCode_t *sourceCode = (crank_talk_SourceCode_t*)sourceCodeValue;
    crank_talk_ScannerState_t state = {
        .sourceCode = sourceCode,
        .text = (const char *)crank_talk_oop_getBytesData((crank_talk_oop_t)sourceCode->text),
        .textSize = crank_talk_object_getSizeInBytes((crank_talk_oop_t)sourceCode->text),
        .position = 0,
        .line = 1,
        .column = 1,
        .isPreviousCR = false,
    };

    for(;;)
    {
        crank_talk_oop_t tokenValue = crank_talk_ScannerState_scanNextToken(context, &state);
        crank_talk_OrderedCollection_add(context, orderedCollection, tokenValue);

        crank_talk_CrankTalkToken_t *token = (crank_talk_CrankTalkToken_t *)tokenValue;
        if(crank_talk_oop_decodeSmallInteger(token->kind) == CrankTalkTokenKindEndOfSource)
            break;
    }

    return crank_talk_OrderedCollection_asArray(context, orderedCollection);
}