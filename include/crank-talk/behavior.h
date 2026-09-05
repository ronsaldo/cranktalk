#ifndef CRANK_TALK_BEHAVIOR_H
#define CRANK_TALK_BEHAVIOR_H

#include "object-model.h"

CRANK_TALK_API void crank_talk_Behavior_computeLayout(crank_talk_VMContext_t *context, crank_talk_oop_t behavior);
CRANK_TALK_API void crank_talk_Behavior_enumerateSlots(crank_talk_VMContext_t *context, crank_talk_oop_t behavior);


#endif //CRANK_TALK_BEHAVIOR_H