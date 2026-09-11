#ifndef CRANK_TALK_ANALYSIS_BUILD_H
#define CRANK_TALK_ANALYSIS_BUILD_H

#include "hir.h"

typedef struct crank_talk_AnalysisAndBuildContext_s crank_talk_AnalysisAndBuildContext_t;

struct crank_talk_AnalysisAndBuildContext_s
{
    crank_talk_Object_t super;
    crank_talk_HIRBuilder_t *builder;
};

CRANK_TALK_API crank_talk_oop_t crank_talk_createAnalysisAndBuildContext(crank_talk_VMContext_t *context, crank_talk_HIRBuilder_t *builder);
CRANK_TALK_API crank_talk_HIRValue_t *crank_talk_ParseTreeAnalyzeAndBuild(crank_talk_VMContext_t *context, crank_talk_oop_t parseTreeNode, crank_talk_oop_t buildContext);

#endif //CRANK_TALK_ANALYSIS_BUILD_H