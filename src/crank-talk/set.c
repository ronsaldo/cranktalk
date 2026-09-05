#include "crank-talk/set.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"

CRANK_TALK_API crank_talk_oop_t
crank_talk_IdentitySet_create(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t setValue = CrankAllocateValuesObjectWithClassName(context, IdentitySet);
    crank_talk_IdentitySet_t *set = (crank_talk_IdentitySet_t*)setValue;
    set->super.super.size = crank_talk_oop_encodeSmallInteger(0);
    return setValue;
}

CRANK_TALK_API bool
crank_talk_IdentitySet_findWithExplicitHash(crank_talk_VMContext_t *context, crank_talk_oop_t setValue, void *element, crank_talk_identitySet_explicitHashFunction_t hashFunction, crank_talk_identitySet_explicitEqualsFunction_t equalsFunction, crank_talk_oop_t *outFoundElement)
{
    *outFoundElement = crank_talk_oop_nil();

    crank_talk_IdentitySet_t *set = (crank_talk_IdentitySet_t*)setValue;
    crank_talk_oop_t storage = (crank_talk_oop_t)set->super.super.storage;
    size_t capacity = crank_talk_object_getSizeInSlots(storage);
    if(capacity == 0)
        return false;

    crank_talk_oop_t *storageValues = crank_talk_oop_getSlotsData(storage);
    size_t hashIndex = hashFunction(context, element) % capacity;
    for(size_t i = hashIndex; i < capacity; ++i)
    {
        crank_talk_oop_t setElement = storageValues[i];
        if(crank_talk_oop_isNil(setElement))
            return false;
        
        if(equalsFunction(element, setElement))
        {
            *outFoundElement = setElement;
            return true;
        }
    }

    for(size_t i = 0; i < hashIndex; ++i)
    {
        crank_talk_oop_t setElement = storageValues[i];
        if(crank_talk_oop_isNil(setElement))
            return false;
        
        if(equalsFunction(element, setElement))
        {
            *outFoundElement = setElement;
            return true;
        }
    }

    return false;
}

static intptr_t
crank_talk_IdentitySet_scanFor(crank_talk_IdentitySet_t *set, crank_talk_oop_t element)
{
    crank_talk_oop_t storage = (crank_talk_oop_t)set->super.super.storage;
    size_t capacity = crank_talk_object_getSizeInSlots(storage);
    if(capacity == 0)
        return -1;

    crank_talk_oop_t *storageValues = crank_talk_oop_getSlotsData(storage);
    size_t hashIndex = crank_talk_oop_getIdentityHash(element) % capacity;
    for(size_t i = hashIndex; i < capacity; ++i) {
        crank_talk_oop_t setElement = storageValues[i];
        if(crank_talk_oop_isNil(setElement) || setElement == element)
            return i;
    }

    for(size_t i = 0; i < hashIndex; ++i) {
        crank_talk_oop_t setElement = storageValues[i];
        if(crank_talk_oop_isNil(setElement) || setElement == element)
            return i;
    }

    return -1;
}

static void
crank_talk_IdentitySet_insertNoCheck(crank_talk_IdentitySet_t *set, crank_talk_oop_t element)
{
    intptr_t elementIndex = crank_talk_IdentitySet_scanFor(set, element);
    assert(elementIndex >= 0);

    crank_talk_oop_t *storageValues = set->super.super.storage->elements;
    storageValues[elementIndex] = element;
}

static void
crank_talk_IdentitySet_increaseCapacity(crank_talk_VMContext_t *context, crank_talk_IdentitySet_t *set)
{
    crank_talk_oop_t oldStorage = (crank_talk_oop_t)set->super.super.storage;
    crank_talk_oop_t *oldStorageValues = crank_talk_oop_getSlotsData(oldStorage);
    size_t oldCapacity = crank_talk_object_getSizeInSlots(oldStorage);
    size_t newCapacity = oldCapacity * 2;
    if(newCapacity < 8)
        newCapacity = 8;

    // Make the new storage
    crank_talk_oop_t newStorage = crank_talk_Array_create(context, newCapacity);
    set->super.super.storage = (crank_talk_Array_t*)newStorage;

    // Reinsert the old elements
    for(size_t i = 0; i < oldCapacity; ++i)
    {
        crank_talk_oop_t oldElement = oldStorageValues[i];
        if(!crank_talk_oop_isNil(oldElement))
            crank_talk_IdentitySet_insertNoCheck(set, oldElement);
    }

}

CRANK_TALK_API void
crank_talk_IdentitySet_insert(crank_talk_VMContext_t *context, crank_talk_oop_t setValue, crank_talk_oop_t element)
{
    crank_talk_IdentitySet_t *set = (crank_talk_IdentitySet_t*)setValue;
    intptr_t elementIndex = crank_talk_IdentitySet_scanFor(set, element);
    if(elementIndex < 0)
    {
        crank_talk_IdentitySet_increaseCapacity(context, set);
        elementIndex = crank_talk_IdentitySet_scanFor(set, element);
        if(elementIndex < 0)
            crank_talk_error("Set is out of memory");
    }

    crank_talk_oop_t *storageValues = set->super.super.storage->elements;
    bool isNewElement = crank_talk_oop_isNil(storageValues[elementIndex]);
    storageValues[elementIndex] = element;

    if(isNewElement)
    {
        size_t capacity = crank_talk_object_getSizeInSlots((crank_talk_oop_t)set->super.super.storage);
        size_t newSize = crank_talk_oop_decodeSmallInteger(set->super.super.size) + 1;
        set->super.super.size = crank_talk_oop_encodeSmallInteger(newSize);
        size_t capacityThreshold = capacity*4/5;
        if(newSize >= capacityThreshold)
            crank_talk_IdentitySet_increaseCapacity(context, set);
    }
}
