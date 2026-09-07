#include "crank-talk/dictionary.h"
#include "crank-talk/array.h"
#include "crank-talk/association.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"

CRANK_TALK_API crank_talk_oop_t
crank_talk_IdentityDictionary_create(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t dictionaryValue = CrankAllocateValuesObjectWithClassName(context, IdentityDictionary);
    crank_talk_IdentityDictionary_t *dictionary = (crank_talk_IdentityDictionary_t*)dictionaryValue;
    dictionary->super.super.size = crank_talk_oop_encodeSmallInteger(0);
    return dictionaryValue;
}

static intptr_t
crank_talk_IdentityDictionary_scanFor(crank_talk_IdentityDictionary_t *dict, crank_talk_oop_t key)
{
    crank_talk_oop_t storage = (crank_talk_oop_t)dict->super.super.storage;
    size_t capacity = crank_talk_object_getSizeInSlots(storage);
    if(capacity == 0)
        return -1;

    crank_talk_oop_t *storageValues = crank_talk_oop_getSlotsData(storage);
    size_t hashIndex = crank_talk_oop_getIdentityHash(key) % capacity;
    for(size_t i = hashIndex; i < capacity; ++i)
    {
        crank_talk_oop_t associationValue = storageValues[i];
        crank_talk_Association_t *association = (crank_talk_Association_t*)associationValue;
        if(crank_talk_oop_isNil(associationValue) || association->super.key == key)
            return i;
    }

    for(size_t i = 0; i < hashIndex; ++i)
    {
        crank_talk_oop_t associationValue = storageValues[i];
        crank_talk_Association_t *association = (crank_talk_Association_t*)associationValue;
        if(crank_talk_oop_isNil(associationValue) || association->super.key == key)
            return i;
    }

    return -1;
}

static void
crank_talk_IdentityDictionary_insertNoCheck(crank_talk_IdentityDictionary_t *dict, crank_talk_oop_t oldAssociationValue)
{
    crank_talk_Association_t *oldAssociation = (crank_talk_Association_t *)oldAssociationValue;
    intptr_t elementIndex =  crank_talk_IdentityDictionary_scanFor(dict, oldAssociation->super.key);
    assert(elementIndex >= 0);

    crank_talk_oop_t *storageValues = crank_talk_oop_getSlotsData((crank_talk_oop_t)dict->super.super.storage);
    storageValues[elementIndex] = oldAssociationValue;
}

static void
crank_talk_IdentityDictionary_increaseCapacity(crank_talk_VMContext_t *context, crank_talk_IdentityDictionary_t *dict)
{
    crank_talk_oop_t oldStorage = (crank_talk_oop_t)dict->super.super.storage;
    crank_talk_oop_t *oldStorageValues = crank_talk_oop_getSlotsData(oldStorage);
    size_t oldCapacity = crank_talk_object_getSizeInSlots(oldStorage);
    size_t newCapacity = oldCapacity * 2;
    if(newCapacity < 8)
        newCapacity = 8;

    // Make the new storage
    crank_talk_oop_t newStorage = crank_talk_Array_create(context, newCapacity);
    dict->super.super.storage = (crank_talk_Array_t*)newStorage;

    // Reinsert the old elements.
    for(size_t i = 0; i < oldCapacity; ++i)
    {
        crank_talk_oop_t oldAssociationValue = oldStorageValues[i];
        if(!crank_talk_oop_isNil(oldAssociationValue))
            crank_talk_IdentityDictionary_insertNoCheck(dict, oldAssociationValue);
    }
}

CRANK_TALK_API
void crank_talk_IdentityDictionary_atPut(crank_talk_VMContext_t *context, crank_talk_oop_t dictionaryValue, crank_talk_oop_t key, crank_talk_oop_t value)
{
    crank_talk_IdentityDictionary_t *dictionary = (crank_talk_IdentityDictionary_t*)dictionaryValue;
    intptr_t elementIndex = crank_talk_IdentityDictionary_scanFor(dictionary, key);
    if(elementIndex < 0)
    {
        crank_talk_IdentityDictionary_increaseCapacity(context, dictionary);
        elementIndex = crank_talk_IdentityDictionary_scanFor(dictionary, key);
        if(elementIndex < 0)
            crank_talk_error("Identity dictionary is out of memory.");
    }

    crank_talk_oop_t *storageValues = dictionary->super.super.storage->elements;
    crank_talk_oop_t oldValue = storageValues[elementIndex];
    if(!crank_talk_oop_isNil(oldValue))
    {
        crank_talk_Association_t *oldAssociation = (crank_talk_Association_t *)oldValue;
        oldAssociation->value = value;
    }
    else
    {
        crank_talk_oop_t newAssociation = crank_talk_Association_create(context, key, value);
        storageValues[elementIndex] = newAssociation;
        size_t capacity = crank_talk_object_getSizeInSlots((crank_talk_oop_t)dictionary->super.super.storage);
        size_t newSize = crank_talk_oop_decodeSmallInteger(dictionary->super.super.size) + 1;
        dictionary->super.super.size = crank_talk_oop_encodeSmallInteger(newSize);
        size_t capacityThreshold = capacity*4/5;
        if(newSize >= capacityThreshold)
            crank_talk_IdentityDictionary_increaseCapacity(context, dictionary);
    }
}

CRANK_TALK_API bool
crank_talk_IdentityDictionary_includesKey(crank_talk_oop_t dictionaryValue, crank_talk_oop_t key)
{
    crank_talk_IdentityDictionary_t *dictionary = (crank_talk_IdentityDictionary_t*)dictionaryValue;
    intptr_t elementIndex = crank_talk_IdentityDictionary_scanFor(dictionary, key);
    if(elementIndex < 0)
        return false;
    
    crank_talk_oop_t *storageValues = dictionary->super.super.storage->elements;
    return !crank_talk_oop_isNil(storageValues[elementIndex]);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_IdentityDictionary_at(crank_talk_oop_t dictionaryValue, crank_talk_oop_t key)
{
    crank_talk_IdentityDictionary_t *dictionary = (crank_talk_IdentityDictionary_t*)dictionaryValue;
    intptr_t elementIndex = crank_talk_IdentityDictionary_scanFor(dictionary, key);
    if(elementIndex < 0)
        crank_talk_error("Key is not found in identity dictionary.");

    crank_talk_oop_t associationValue = dictionary->super.super.storage->elements[elementIndex];
    if(crank_talk_oop_isNil(associationValue))
        crank_talk_error("Key is not found in identity dictionary.");

    crank_talk_Association_t *association = (crank_talk_Association_t*)associationValue;
    return association->value;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_SystemEnvironment_create(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t dictionaryValue = CrankAllocateValuesObjectWithClassName(context, SystemEnvironment);
    crank_talk_IdentityDictionary_t *dictionary = (crank_talk_IdentityDictionary_t*)dictionaryValue;
    dictionary->super.super.size = crank_talk_oop_encodeSmallInteger(0);
    return dictionaryValue;
}

CRANK_TALK_API void
crank_talk_SystemEnvironment_atPut(crank_talk_VMContext_t *context, crank_talk_oop_t dictionaryValue, crank_talk_oop_t key, crank_talk_oop_t value)
{
    if(!crank_talk_oop_isKindOf(context, key, context->roots.SymbolClass))
        crank_talk_error("System environment expects the keys to be symbols.");
    
    crank_talk_IdentityDictionary_t *dictionary = (crank_talk_IdentityDictionary_t*)dictionaryValue;
    intptr_t elementIndex = crank_talk_IdentityDictionary_scanFor(dictionary, key);
    if(elementIndex < 0)
    {
        crank_talk_IdentityDictionary_increaseCapacity(context, dictionary);
        elementIndex = crank_talk_IdentityDictionary_scanFor(dictionary, key);
        if(elementIndex < 0)
            crank_talk_error("Identity dictionary is out of memory.");
    }

    crank_talk_oop_t *storageValues = dictionary->super.super.storage->elements;
    crank_talk_oop_t oldValue = storageValues[elementIndex];
    if(!crank_talk_oop_isNil(oldValue))
    {
        crank_talk_Association_t *oldAssociation = (crank_talk_Association_t *)oldValue;
        oldAssociation->value = value;
    }
    else
    {
        crank_talk_oop_t newGlobalVariableValue = CrankAllocateValuesObjectWithClassName(context, GlobalVariable);
        crank_talk_GlobalVariable_t *newGlobalVariable = (crank_talk_GlobalVariable_t *)newGlobalVariableValue;
        newGlobalVariable->super.super.name = (crank_talk_Symbol_t*)key;
        newGlobalVariable->super.value = value;
        storageValues[elementIndex] = newGlobalVariableValue;

        size_t capacity = crank_talk_object_getSizeInSlots((crank_talk_oop_t)dictionary->super.super.storage);
        size_t newSize = crank_talk_oop_decodeSmallInteger(dictionary->super.super.size) + 1;
        dictionary->super.super.size = crank_talk_oop_encodeSmallInteger(newSize);
        size_t capacityThreshold = capacity*4/5;
        if(newSize >= capacityThreshold)
            crank_talk_IdentityDictionary_increaseCapacity(context, dictionary);
    }
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_MethodDictionary_create(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t dictionaryValue = CrankAllocateValuesObjectWithClassName(context, MethodDictionary);
    crank_talk_MethodDictionary_t *dictionary = (crank_talk_MethodDictionary_t*)dictionaryValue;
    dictionary->super.super.size = crank_talk_oop_encodeSmallInteger(0);
    return dictionaryValue;
}

static intptr_t
crank_talk_MethodDictionary_scanFor(crank_talk_MethodDictionary_t *dict, crank_talk_oop_t key)
{
    crank_talk_oop_t storage = (crank_talk_oop_t)dict->super.super.storage;
    size_t capacity =  crank_talk_object_getSizeInSlots(storage) / 2;
    if(capacity == 0)
        return -1;

    crank_talk_oop_t *storageValues = crank_talk_oop_getSlotsData(storage);
    size_t hashIndex = crank_talk_oop_getIdentityHash(key) % capacity;
    for(size_t i = hashIndex; i < capacity; ++i)
    {
        crank_talk_oop_t dictionaryKey = storageValues[i*2];
        if(crank_talk_oop_isNil(dictionaryKey) || dictionaryKey == key)
            return i;
    }

    for(size_t i = 0; i < hashIndex; ++i)
    {
        crank_talk_oop_t dictionaryKey = storageValues[i*2];
        if(crank_talk_oop_isNil(dictionaryKey) || dictionaryKey == key)
            return i;
    }

    return -1;
}

static void
crank_talk_MethodDictionary_insertNoCheck(crank_talk_MethodDictionary_t *dict, crank_talk_oop_t key, crank_talk_oop_t value)
{
    intptr_t elementIndex = crank_talk_MethodDictionary_scanFor(dict, key);
    assert(elementIndex >= 0);
    crank_talk_oop_t *storage = dict->super.super.storage->elements;
    storage[elementIndex*2] = key;
    storage[elementIndex*2 + 1] = value;
}

static void
crank_talk_MethodDictionary_increaseCapacity(crank_talk_VMContext_t *context, crank_talk_MethodDictionary_t *dict)
{
    crank_talk_oop_t oldStorage = (crank_talk_oop_t)dict->super.super.storage;
    size_t oldCapacity = crank_talk_object_getSizeInSlots(oldStorage) / 2;
    size_t newCapacity = oldCapacity*2;
    if(newCapacity < 8)
        newCapacity = 8;

    // Make the new storage
    crank_talk_oop_t newStorage = crank_talk_Array_create(context, newCapacity*2);
    dict->super.super.storage = (crank_talk_Array_t*)newStorage;

    // Reinsert the old elements
    crank_talk_oop_t *oldStorageElements = crank_talk_oop_getSlotsData(oldStorage);
    for(size_t i = 0; i < oldCapacity; ++i)
    {
        crank_talk_oop_t key = oldStorageElements[i*2];
        crank_talk_oop_t value = oldStorageElements[i*2 + 1];
        if(!crank_talk_oop_isNil(key))
            crank_talk_MethodDictionary_insertNoCheck(dict, key, value);
    }
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_MethodDictionary_atOrNil(crank_talk_oop_t dictValue, crank_talk_oop_t key)
{
    crank_talk_MethodDictionary_t *dict = (crank_talk_MethodDictionary_t *)dictValue;
    intptr_t elementIndex = crank_talk_MethodDictionary_scanFor(dict, key);
    if(elementIndex < 0)
        return crank_talk_oop_nil();

    crank_talk_oop_t *storageElements = dict->super.super.storage->elements;
    crank_talk_oop_t oldKey = storageElements[elementIndex*2];
    if(!crank_talk_oop_isNil(oldKey))
        return storageElements[elementIndex*2 + 1];
    return crank_talk_oop_nil();
}

CRANK_TALK_API void
crank_talk_MethodDictionary_atPut(crank_talk_VMContext_t *context, crank_talk_oop_t dictValue, crank_talk_oop_t key, crank_talk_oop_t value)
{
    crank_talk_MethodDictionary_t *dict = (crank_talk_MethodDictionary_t *)dictValue;
    intptr_t elementIndex = crank_talk_MethodDictionary_scanFor(dict, key);
    if(elementIndex < 0)
    {
        crank_talk_MethodDictionary_increaseCapacity(context, dict);
        elementIndex = crank_talk_MethodDictionary_scanFor(dict, key);
        if(elementIndex < 0)
            crank_talk_error("Method dictionary is out of memory");
    }
    
    crank_talk_oop_t *storageElements = dict->super.super.storage->elements;
    crank_talk_oop_t oldKey = storageElements[elementIndex*2];
    if(!crank_talk_oop_isNil(oldKey))
    {
        storageElements[elementIndex*2 + 1] = value;
    }
    else
    {
        storageElements[elementIndex*2] = key;
        storageElements[elementIndex*2 + 1] = value;

        size_t capacity = crank_talk_object_getSizeInSlots((crank_talk_oop_t)dict->super.super.storage) / 2;
        size_t newSize = crank_talk_oop_decodeSmallInteger(dict->super.super.size) + 1;
        dict->super.super.size = crank_talk_oop_encodeSmallInteger(newSize);
        size_t capacityThreshold = capacity*4/5;
        if(newSize >= capacityThreshold)
            crank_talk_MethodDictionary_increaseCapacity(context, dict);
    }
}
