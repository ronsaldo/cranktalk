#ifndef CRANK_TALK_EVAL_H
#define CRANK_TALK_EVAL_H

#include "object-model.h"

CRANK_TALK_API crank_talk_oop_t crank_talk_analyzeAndEvaluateSourceCode(crank_talk_VMContext_t *context, crank_talk_oop_t sourceCodeValue);
CRANK_TALK_API crank_talk_oop_t crank_talk_analyzeAndEvaluateString(crank_talk_VMContext_t *context, const char *sourceString);
CRANK_TALK_API crank_talk_oop_t crank_talk_analyzeAndEvaluateFileNamed(crank_talk_VMContext_t *context, crank_talk_oop_t fileNameString);

#endif //CRANK_TALK_EVAL_H