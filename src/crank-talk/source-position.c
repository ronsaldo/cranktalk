#include "crank-talk/source-position.h"
#include "crank-talk/context.h"

CRANK_TALK_API crank_talk_oop_t
crank_talk_SourcePosition_to(crank_talk_VMContext_t *context, crank_talk_oop_t start, crank_talk_oop_t end)
{
    crank_talk_oop_t result = CrankAllocateValuesObjectWithClassName(context, SourcePosition);

    crank_talk_SourcePosition_t *resultPosition = (crank_talk_SourcePosition_t*)result;
    crank_talk_SourcePosition_t *startPosition = (crank_talk_SourcePosition_t*)start;
    crank_talk_SourcePosition_t *endPosition = (crank_talk_SourcePosition_t*)end;
    resultPosition->sourceCode = startPosition->sourceCode;

    resultPosition->startIndex = startPosition->startLine;
    resultPosition->endIndex = endPosition->endIndex;

    resultPosition->startLine = startPosition->startLine;
    resultPosition->endLine = endPosition->endLine;

    resultPosition->startColumn = startPosition->startColumn;
    resultPosition->endColumn = endPosition->endColumn;

    return result;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_SourcePosition_until(crank_talk_VMContext_t *context, crank_talk_oop_t start, crank_talk_oop_t end)
{
    crank_talk_oop_t result = CrankAllocateValuesObjectWithClassName(context, SourcePosition);

    crank_talk_SourcePosition_t *resultPosition = (crank_talk_SourcePosition_t*)result;
    crank_talk_SourcePosition_t *startPosition = (crank_talk_SourcePosition_t*)start;
    crank_talk_SourcePosition_t *endPosition = (crank_talk_SourcePosition_t*)end;
    resultPosition->sourceCode = startPosition->sourceCode;

    resultPosition->startIndex = startPosition->startLine;
    resultPosition->endIndex = endPosition->startLine;

    resultPosition->startLine = startPosition->startLine;
    resultPosition->endLine = endPosition->startLine;

    resultPosition->startColumn = startPosition->startColumn;
    resultPosition->endColumn = endPosition->startColumn;

    return result;
}
