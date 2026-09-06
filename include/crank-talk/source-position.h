#ifndef CRANK_TALK_SOURCE_POSITION_H
#define CRANK_TALK_SOURCE_POSITION_H

#include "object-model.h"

typedef struct crank_talk_StringSlice_s
{
    size_t size;
    const char *text;
} crank_talk_StringSlice_t;

CRANK_TALK_INLINE crank_talk_StringSlice_t
crank_talk_SourcePosition_asStringSlice(crank_talk_oop_t sourcePositionValue)
{
    crank_talk_SourcePosition_t *sourcePosition = (crank_talk_SourcePosition_t*)sourcePositionValue;
    crank_talk_SourceCode_t *sourceCode = (crank_talk_SourceCode_t*)sourcePosition->sourceCode;
    const char *text = sourceCode->text->elements;

    crank_talk_StringSlice_t slice = {
        .size = crank_talk_oop_decodeSmallInteger(sourcePosition->endIndex) - crank_talk_oop_decodeSmallInteger(sourcePosition->startIndex),
        .text = text + crank_talk_oop_decodeSmallInteger(sourcePosition->startIndex),
    };
    return slice;
}

CRANK_TALK_API crank_talk_oop_t crank_talk_SourcePosition_to(crank_talk_VMContext_t *context, crank_talk_oop_t start, crank_talk_oop_t end);
CRANK_TALK_API crank_talk_oop_t crank_talk_SourcePosition_until(crank_talk_VMContext_t *context, crank_talk_oop_t start, crank_talk_oop_t end);

#endif //CRANK_TALK_SOURCE_POSITION_H