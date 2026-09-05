#ifndef CRANK_TALK_GC_H
#define CRANK_TALK_GC_H

#include "stack-frame.h"

/**
 * Runs the garbage collector right now.
 */
CRANK_TALK_API void crank_talk_gc_collect(crank_talk_VMContext_t *context);

/**
 * A safepoint for the GC
 */
CRANK_TALK_API void crank_talk_gc_safepoint(crank_talk_VMContext_t *context);

/**
 * Iterate trough all the roots in the system.
 */
CRANK_TALK_API void crank_talk_gc_iterateRoots(crank_talk_VMContext_t *context, void *userdata, crank_talk_GCRootIterationFunction_t iterationFunction);

#endif //CRANK_TALK_GC_H
