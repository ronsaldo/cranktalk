#include "crank-talk/array.h"
#include "crank-talk/context.h"

CRANK_TALK_API crank_talk_oop_t
crank_talk_Array_create(crank_talk_VMContext_t *context, size_t slotCount)
{
    return CrankAllocateVariadicObjectWithClassName(context, Array, CrankTalkGCObjectDataTypeOops, sizeof(crank_talk_oop_t)*slotCount);
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Array_withValues(crank_talk_VMContext_t *context, size_t valueCount, crank_talk_oop_t *values)
{
    crank_talk_oop_t arrayValue = crank_talk_Array_create(context, valueCount);
    crank_talk_oop_t *arrayValues = crank_talk_oop_getSlotsData(arrayValue);
    for(size_t i = 0; i < valueCount; ++i)
        arrayValues[i] = values[i];

    return arrayValue;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Array_concat(crank_talk_VMContext_t *context, crank_talk_oop_t left, crank_talk_oop_t right)
{
    crank_talk_oop_t *leftValues = crank_talk_oop_getSlotsData(left);
    crank_talk_oop_t *rightValues = crank_talk_oop_getSlotsData(right);
    size_t leftSize = crank_talk_object_getSizeInSlots(left);
    size_t rightSize = crank_talk_object_getSizeInSlots(right);

    crank_talk_oop_t result = crank_talk_Array_create(context, leftSize + rightSize);
    crank_talk_oop_t *resultValues = crank_talk_oop_getSlotsData(result);
    for(size_t i = 0; i < leftSize; ++i)
        resultValues[i] = leftValues[i];
    for(size_t i = 0; i < rightSize; ++i)
        resultValues[leftSize + i] = rightValues[i];

    return result;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Array_copyWith(crank_talk_VMContext_t *context, crank_talk_oop_t array, crank_talk_oop_t element)
{
    crank_talk_oop_t *arrayValues = crank_talk_oop_getSlotsData(array);
    size_t arraySize = crank_talk_object_getSizeInSlots(array);
    for(size_t i = 0; i < arraySize; ++i)
    {
        if(arrayValues[i] == element)
            return array;
    }

    crank_talk_oop_t newArray = crank_talk_Array_create(context, arraySize + 1);
    crank_talk_oop_t *newArrayValues = crank_talk_oop_getSlotsData(newArray);
    for(size_t i = 0; i < arraySize; ++i)
        newArrayValues[i] = arrayValues[i];
    newArrayValues[arraySize] = element;
    return newArray;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Array_copyWithout(crank_talk_VMContext_t *context, crank_talk_oop_t array, crank_talk_oop_t element)
{
    // Compute the new array size.
    crank_talk_oop_t *arrayValues = crank_talk_oop_getSlotsData(array);
    size_t arraySize = crank_talk_object_getSizeInSlots(array);
    size_t resultSize = 0;
    for(size_t i = 0; i < arraySize; ++i)
    {
        if(arrayValues[i] != element)
            ++resultSize;
    }

    // Create the new array
    crank_talk_oop_t newArray = crank_talk_Array_create(context, resultSize);
    crank_talk_oop_t *newArrayValues = crank_talk_oop_getSlotsData(newArray);
    size_t destIndex = 0;
    for(size_t i = 0; i < arraySize; ++i)
    {
        if(arrayValues[i] != element)
            newArrayValues[destIndex++] = arrayValues[i];
    }
        
    return newArray;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_Array_With(crank_talk_VMContext_t *context, crank_talk_oop_t first)
{
    crank_talk_oop_t array = crank_talk_Array_create(context, 1);
    crank_talk_oop_t *arrayData = crank_talk_oop_getSlotsData(array);
    arrayData[0] = first;
    return array;
}
CRANK_TALK_API crank_talk_oop_t
crank_talk_Array_WithWith(crank_talk_VMContext_t *context, crank_talk_oop_t first, crank_talk_oop_t second)
{
    crank_talk_oop_t array = crank_talk_Array_create(context, 2);
    crank_talk_oop_t *arrayData = crank_talk_oop_getSlotsData(array);
    arrayData[0] = first;
    arrayData[1] = second;
    return array;
}