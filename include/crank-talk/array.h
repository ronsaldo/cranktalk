#ifndef CRANK_TALK_ARRAY_H
#define CRANK_TALK_ARRAY_H

#include "object-model.h"

CRANK_TALK_API crank_talk_oop_t crank_talk_Array_create(crank_talk_VMContext_t *context, size_t slotCount);
CRANK_TALK_API crank_talk_oop_t crank_talk_Array_withValues(crank_talk_VMContext_t *context, size_t valueCount, crank_talk_oop_t *values);

CRANK_TALK_API crank_talk_oop_t crank_talk_Array_concat(crank_talk_VMContext_t *context, crank_talk_oop_t left, crank_talk_oop_t right);
CRANK_TALK_API crank_talk_oop_t crank_talk_Array_copyWith(crank_talk_VMContext_t *context, crank_talk_oop_t array, crank_talk_oop_t element);
CRANK_TALK_API crank_talk_oop_t crank_talk_Array_With(crank_talk_VMContext_t *context, crank_talk_oop_t first);
CRANK_TALK_API crank_talk_oop_t crank_talk_Array_WithWith(crank_talk_VMContext_t *context, crank_talk_oop_t first, crank_talk_oop_t second);

#endif //CRANK_TALK_ARRAY_H