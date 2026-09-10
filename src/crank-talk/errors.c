#include "crank-talk/errors.h"
#include "crank-talk/context.h"
#include "crank-talk/source-position.h"
#include "crank-talk/string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

CRANK_TALK_API void
crank_talk_error(const char *message)
{
    fprintf(stderr, "Error: %s\n", message);
    abort();
}

CRANK_TALK_API void
crank_talk_errorPrintf(crank_talk_VMContext_t *context, const char *format, ...)
{
    (void)context;
    char buffer[256];

    va_list valist;
    va_start(valist, format);
    vsnprintf(buffer, sizeof(buffer), format, valist);
    va_end(valist);

    crank_talk_error(buffer);
}

CRANK_TALK_API void
crank_talk_errorAtSourcePosition(crank_talk_VMContext_t *context, crank_talk_oop_t abstractSourcePosition, const char *message)
{
    if(crank_talk_oop_isKindOf(context, abstractSourcePosition, context->roots.SourcePositionClass))
    {
        crank_talk_SourcePosition_t *sourcePosition = (crank_talk_SourcePosition_t*)abstractSourcePosition;
        crank_talk_SourceCode_t *sourceCode = (crank_talk_SourceCode_t *)sourcePosition->sourceCode;
        fprintf(stderr, CRANK_TALK_STRING_FORMAT "/" CRANK_TALK_STRING_FORMAT ":%d.%d-%d.%d: ",
            CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)sourceCode->directory),
            CRANK_TALK_STRING_FORMAT_ARG((crank_talk_oop_t)sourceCode->name),
            (int)crank_talk_oop_decodeSmallInteger(sourcePosition->startLine), (int)crank_talk_oop_decodeSmallInteger(sourcePosition->startColumn),
            (int)crank_talk_oop_decodeSmallInteger(sourcePosition->endLine), (int)crank_talk_oop_decodeSmallInteger(sourcePosition->endColumn));

    }

    fprintf(stderr, "Error: %s\n", message);
    abort();
}

CRANK_TALK_API void
crank_talk_errorPrintfAtSourcePosition(crank_talk_VMContext_t *context, crank_talk_oop_t sourcePosition, const char *format, ...)
{
    char buffer[256];

    va_list valist;
    va_start(valist, format);
    vsnprintf(buffer, sizeof(buffer), format, valist);
    va_end(valist);

    crank_talk_errorAtSourcePosition(context, sourcePosition, buffer);
}
