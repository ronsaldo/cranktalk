#ifndef CRANK_TALK_ERRORS_H
#define CRANK_TALK_ERRORS_H

#pragma once

#include "object-model.h"

typedef struct crank_talk_Exception_s crank_talk_Exception_t;
typedef struct crank_talk_Error_s crank_talk_Error_t;
typedef struct crank_talk_MessageNotUnderstood_s crank_talk_MessageNotUnderstood_t;

struct crank_talk_Exception_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t messageText;
    crank_talk_oop_t resumeValue;
};

struct crank_talk_Error_s
{
    crank_talk_Exception_t super;
};

struct crank_talk_MessageNotUnderstood_s
{
    crank_talk_Error_t super;
    crank_talk_oop_t message;
};


CRANK_TALK_API void crank_talk_Exception_signal(crank_talk_VMContext_t *context, crank_talk_Exception_t *exception);

CRANK_TALK_API void crank_talk_error(crank_talk_VMContext_t *context, const char *message);
CRANK_TALK_API void crank_talk_errorPrintf(crank_talk_VMContext_t *context, const char *format, ...);
CRANK_TALK_API void crank_talk_errorAtSourcePosition(crank_talk_VMContext_t *context, crank_talk_oop_t sourcePosition, const char *message);
CRANK_TALK_API void crank_talk_errorPrintfAtSourcePosition(crank_talk_VMContext_t *context, crank_talk_oop_t sourcePosition, const char *format, ...);

#endif //CRANK_TALK_ERRORS_H