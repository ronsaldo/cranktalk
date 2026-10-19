#include "crank-talk/ordered-collection.h"
#include "crank-talk/array.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include <stdio.h>

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

CRANK_TALK_API crank_talk_oop_t
crank_talk_OrderedCollection_at(crank_talk_VMContext_t *context, crank_talk_oop_t orderedCollectionValue, size_t index)
{
    (void)context;
    crank_talk_OrderedCollection_t *collection = (crank_talk_OrderedCollection_t*)orderedCollectionValue;
    size_t size = crank_talk_oop_decodeSmallInteger(collection->size);
    if(index < 1 || index > size)
        crank_talk_error(context, "Ordered collection index is out of bounds.");
    
    return collection->storage->elements[index - 1];
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_OrderedCollection_atPut(crank_talk_VMContext_t *context, crank_talk_oop_t orderedCollectionValue, size_t index, crank_talk_oop_t value)
{
    (void)context;
    crank_talk_OrderedCollection_t *collection = (crank_talk_OrderedCollection_t*)orderedCollectionValue;
    size_t size = crank_talk_oop_decodeSmallInteger(collection->size);
    if(index < 1 || index > size)
        crank_talk_error(context, "Ordered collection index is out of bounds.");
    
    collection->storage->elements[index - 1] = value;
    return value;
}

static void
crank_talk_OrderedCollection_initialize_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t orderedCollectionValue = primContext->arguments[0];
    crank_talk_OrderedCollection_t *orderedCollection = (crank_talk_OrderedCollection_t *)orderedCollectionValue;
    orderedCollection->size = crank_talk_oop_encodeSmallInteger(0);
    return crank_talk_Primitive_return(primContext, orderedCollectionValue);
}

static void
crank_talk_OrderedCollection_add_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t orderedCollection = primContext->arguments[0];
    crank_talk_oop_t element = primContext->arguments[1];
    crank_talk_OrderedCollection_add(primContext->context, orderedCollection, element);
    return crank_talk_Primitive_return(primContext, element);
}

static void
crank_talk_OrderedCollection_asArray_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t orderedCollection = primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, crank_talk_OrderedCollection_asArray(primContext->context, orderedCollection));
}


static void
crank_talk_OrderedCollection_at_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t orderedCollection = primContext->arguments[0];
    crank_talk_oop_t index = primContext->arguments[1];
    if(!crank_talk_oop_isSmallInteger(index))
        crank_talk_error(primContext->context, "Expected a SmallInteger index for OrderedCollection >> #at:");

    crank_talk_oop_t value = crank_talk_OrderedCollection_at(primContext->context, orderedCollection, crank_talk_oop_decodeSmallInteger(index));
    return crank_talk_Primitive_return(primContext, value);
}

static void
crank_talk_OrderedCollection_atPut_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t orderedCollection = primContext->arguments[0];
    crank_talk_oop_t index = primContext->arguments[1];
    crank_talk_oop_t value = primContext->arguments[2];
    if(!crank_talk_oop_isSmallInteger(index))
        crank_talk_error(primContext->context, "Expected a SmallInteger index for OrderedCollection >> #at:put:");

    crank_talk_oop_t result = crank_talk_OrderedCollection_atPut(primContext->context, orderedCollection, crank_talk_oop_decodeSmallInteger(index), value);
    return crank_talk_Primitive_return(primContext, result);
}

static void
crank_talk_OrderedCollection_size_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_OrderedCollection_t *orderedCollection = (crank_talk_OrderedCollection_t *)primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, orderedCollection->size);
}

void
crank_talk_registerOrderedCollectionPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OrderedCollectionClass, "initialize", 1, "OrderedCollection::initialize", crank_talk_OrderedCollection_initialize_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OrderedCollectionClass, "add:",       2, "OrderedCollection::add",        crank_talk_OrderedCollection_add_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OrderedCollectionClass, "asArray",    1, "OrderedCollection::asArray",    crank_talk_OrderedCollection_asArray_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OrderedCollectionClass, "at:",        2, "OrderedCollection::at:",        crank_talk_OrderedCollection_at_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OrderedCollectionClass, "at:put:",    3, "OrderedCollection::at:put:",    crank_talk_OrderedCollection_atPut_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.OrderedCollectionClass, "size",       1, "OrderedCollection::size",       crank_talk_OrderedCollection_size_primitive);
}