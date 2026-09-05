#ifndef CRANK_TALK_ERRORS_H
#define CRANK_TALK_ERRORS_H

#pragma once

#include "object-model.h"

CRANK_TALK_API void crank_talk_error(const char *message);
CRANK_TALK_API void crank_talk_errorPrintf(crank_talk_VMContext_t *context, const char *format, ...);
CRANK_TALK_API void crank_talk_errorAtSourcePosition(crank_talk_VMContext_t *context, crank_talk_oop_t sourcePosition, const char *message);
CRANK_TALK_API void crank_talk_errorPrintfAtSourcePosition(crank_talk_VMContext_t *context, crank_talk_oop_t sourcePosition, const char *format, ...);

#endif //CRANK_TALK_ERRORS_H