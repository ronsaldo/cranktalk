#include "crank-talk/ordered-collection.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"

CRANK_TALK_API crank_talk_oop_t
crank_talk_OrderedCollection_create(crank_talk_VMContext_t *context)
{
    crank_talk_oop_t collectionValue = CrankAllocateValuesObjectWithClassName(context, OrderedCollection);
    crank_talk_OrderedCollection_t *collection = (crank_talk_OrderedCollection_t*)collectionValue;
    collection->size = crank_talk_oop_encodeSmallInteger(0);
    return collectionValue;
}

CRANK_TALK_API size_t
crank_talk_OrderedCollection_getSize(crank_talk_oop_t orderedCollectionValue)
{
    crank_talk_OrderedCollection_t *collection = (crank_talk_OrderedCollection_t*)orderedCollectionValue;
    return crank_talk_oop_decodeSmallInteger(collection->size);
}

static void
crank_talk_OrderedCollection_increaseCapacity(crank_talk_VMContext_t *context, crank_talk_OrderedCollection_t *collection)
{
    size_t size = crank_talk_oop_decodeSmallInteger(collection->size);
    size_t oldCapacity = crank_talk_object_getSizeInSlots((crank_talk_oop_t)collection->storage);
    size_t newCapacity = oldCapacity*2;
    if(newCapacity < 4)
        newCapacity = 4;

    crank_talk_oop_t *oldStorageValues = collection->storage->elements;
    crank_talk_oop_t newStorage = crank_talk_Array_create(context, newCapacity);
    crank_talk_oop_t *newStorageValues = crank_talk_oop_getSlotsData(newStorage);
    for(size_t i = 0; i < size; ++i)
        newStorageValues[i] = oldStorageValues[i];

    collection->storage = (crank_talk_Array_t*)newStorage;
}

CRANK_TALK_API void
crank_talk_OrderedCollection_add(crank_talk_VMContext_t *context, crank_talk_oop_t orderedCollectionValue, crank_talk_oop_t value)
{
    crank_talk_OrderedCollection_t *collection = (crank_talk_OrderedCollection_t*)orderedCollectionValue;
    size_t size = crank_talk_oop_decodeSmallInteger(collection->size);
    size_t capacity = crank_talk_object_getSizeInSlots((crank_talk_oop_t)collection->storage);
    if(size >= capacity)
        crank_talk_OrderedCollection_increaseCapacity(context, collection);

    collection->storage->elements[size] = value;
    collection->size = crank_talk_oop_encodeSmallInteger(size + 1);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_OrderedCollection_asArray(crank_talk_VMContext_t *context, crank_talk_oop_t orderedCollectionValue)
{
    crank_talk_OrderedCollection_t *collection = (crank_talk_OrderedCollection_t*)orderedCollectionValue;
    size_t size = crank_talk_oop_decodeSmallInteger(collection->size);
    crank_talk_oop_t arrayValue = crank_talk_Array_create(context, size);
    crank_talk_oop_t *arrayValues = crank_talk_oop_getSlotsData(arrayValue);
    crank_talk_oop_t *sourceArrayValues = collection->storage->elements;
    for(size_t i = 0; i < size; ++i)
        arrayValues[i] = sourceArrayValues[i];
    return arrayValue;
}

CRANK_TALK_API crank_talk_oop_t crank_talk_OrderedCollection_at(crank_talk_VMContext_t *context, crank_talk_oop_t orderedCollectionValue, size_t index)
{
    (void)context;
    crank_talk_OrderedCollection_t *collection = (crank_talk_OrderedCollection_t*)orderedCollectionValue;
    size_t size = crank_talk_oop_decodeSmallInteger(collection->size);
    if(index >= size)
        crank_talk_error("Ordered collection index is out of bounds.");
    
    return collection->storage->elements[index];
}
