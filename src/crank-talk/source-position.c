#include "crank-talk/source-position.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"

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

static crank_talk_oop_t
crank_talk_SourcePosition_printOn_primitive(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 1);
    crank_talk_oop_t receiver = arguments[0];
    crank_talk_oop_t stream = arguments[1];
    crank_talk_SourcePosition_t *sourcePosition = (crank_talk_SourcePosition_t*)receiver;

    if(sourcePosition->sourceCode->directory)
    {
        crank_talk_StringStream_insertBytesObject(context, stream, (crank_talk_oop_t)sourcePosition->sourceCode->directory);
        crank_talk_StringStream_insertCString(context, stream, "/");
    }        
    if(sourcePosition->sourceCode->name)
    {
        crank_talk_StringStream_insertBytesObject(context, stream, (crank_talk_oop_t)sourcePosition->sourceCode->name);
    }

    crank_talk_StringStream_insertCString(context, stream, ":");
    crank_talk_StringStream_print(context, stream, sourcePosition->startLine);
    crank_talk_StringStream_insertCString(context, stream, ".");
    crank_talk_StringStream_print(context, stream, sourcePosition->startColumn);

    crank_talk_StringStream_insertCString(context, stream, "-");

    crank_talk_StringStream_print(context, stream, sourcePosition->endLine);
    crank_talk_StringStream_insertCString(context, stream, ".");
    crank_talk_StringStream_print(context, stream, sourcePosition->endColumn);
    return receiver;
}

void crank_talk_registerSourcePositionPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetNativePrimitiveMethod(context, context->roots.SourcePositionClass, "printOn:", 2, crank_talk_SourcePosition_printOn_primitive);
}