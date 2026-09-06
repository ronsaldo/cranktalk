#include "crank-talk/behavior.h"
#include "crank-talk/array.h"
#include "crank-talk/dictionary.h"

CRANK_TALK_API void
crank_talk_Behavior_computeLayout(crank_talk_VMContext_t *context, crank_talk_oop_t behaviorValue)
{
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t*)behaviorValue;
    assert(!crank_talk_oop_isNil(behavior->slots));
    if(!crank_talk_oop_isNil(behavior->allSlots))
        return;

    // Add the super type slots.
    crank_talk_oop_t allSlots = behavior->slots;
    crank_talk_oop_t superBehaviorValue = (crank_talk_oop_t)behavior->superclass;
    if(!crank_talk_oop_isNil(superBehaviorValue)) {
        crank_talk_Behavior_t *superBehavior = (crank_talk_Behavior_t*)superBehaviorValue;
        crank_talk_Behavior_computeLayout(context,superBehaviorValue);
        allSlots = crank_talk_Array_concat(context, superBehavior->allSlots, behavior->slots);
    }

    behavior->allSlots = allSlots;
    size_t allSlotCount = crank_talk_object_getSizeInSlots(allSlots);
    behavior->instanceSize = crank_talk_oop_encodeSmallInteger(sizeof(crank_talk_ObjectHeader_t) + allSlotCount*sizeof(crank_talk_oop_t));
    crank_talk_Behavior_enumerateSlots(context, behaviorValue);
}

CRANK_TALK_API void
crank_talk_Behavior_enumerateSlots(crank_talk_VMContext_t *context, crank_talk_oop_t behaviorValue)
{
    (void)context;
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t*)behaviorValue;
    assert(!crank_talk_oop_isNil(behavior->allSlots));

    size_t slotCount = crank_talk_object_getSizeInSlots(behavior->allSlots);
    crank_talk_oop_t *slots = crank_talk_oop_getSlotsData(behavior->allSlots);
    for(size_t i = 0; i < slotCount; ++i)
    {
        crank_talk_Slot_t *slot = (crank_talk_Slot_t *)slots[i];
        slot->index = crank_talk_oop_encodeSmallInteger(i);
    }
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Behavior_lookupSelector(crank_talk_oop_t behaviorValue, crank_talk_oop_t selector)
{
    crank_talk_Behavior_t *behavior = (crank_talk_Behavior_t*)behaviorValue;
    while(behavior)
    {
        if(behavior->methodDictionary)
        {
            crank_talk_oop_t foundMethod = crank_talk_MethodDictionary_atOrNil((crank_talk_oop_t)behavior->methodDictionary, selector);
            if(!crank_talk_oop_isNil(foundMethod))
                return foundMethod;
        }

        behavior = behavior->superclass;
    }

    return crank_talk_oop_nil();
}