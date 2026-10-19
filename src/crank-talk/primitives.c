#include "crank-talk/primitives.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

CRANK_TALK_API void
crank_talk_PrimitiveTables_initialize(crank_talk_PrimitivesTable_t* table)
{
    table->capacity = 1024;
    table->size = 0;
    table->entries = calloc(table->capacity, sizeof(crank_talk_PrimitiveTableEntry_t));
}

CRANK_TALK_API void
crank_talk_PrimitiveTables_destroy(crank_talk_PrimitivesTable_t* table)
{
    free(table->entries);
}

static intptr_t
crank_talk_PrimitiveTables_scanFor(crank_talk_PrimitivesTable_t* table, size_t primitiveNameSize, const char *primitiveName)
{
    size_t hash = crank_talk_String_computeHashWithBytes(primitiveNameSize, (const uint8_t*)primitiveName);
    size_t hashIndex = hash % table->capacity;
    for(size_t i = hashIndex; i < table->capacity; ++i)
    {
        crank_talk_PrimitiveTableEntry_t *entry = table->entries + i;
        if(!entry->name || (strlen(entry->name) == primitiveNameSize && memcmp(entry->name, primitiveName, primitiveNameSize) == 0))
            return i;
    }
    for(size_t i = 0; i < hashIndex; ++i)
    {
        crank_talk_PrimitiveTableEntry_t *entry = table->entries + i;
        if(!entry->name || (strlen(entry->name) == primitiveNameSize && memcmp(entry->name, primitiveName, primitiveNameSize) == 0))
            return i;
    }

    return -1;
}

CRANK_TALK_API void
crank_talk_PrimitiveTables_register(crank_talk_PrimitivesTable_t* table, const char *primitiveName, crank_talk_PrimitiveFunction_t primitiveFunction)
{
    intptr_t scanIndex = crank_talk_PrimitiveTables_scanFor(table, strlen(primitiveName), primitiveName);
    if(scanIndex < 0)
    {
        fprintf(stderr, "Reached the capacity of the primitive table.\n");
        abort();
    }

    crank_talk_PrimitiveTableEntry_t *entry = table->entries + scanIndex;
    entry->name = primitiveName;
    entry->primitive = primitiveFunction;
    ++table->size;
}

CRANK_TALK_API crank_talk_PrimitiveFunction_t
crank_talk_PrimitiveTables_lookupWithCString(crank_talk_PrimitivesTable_t* table, const char *primitiveName)
{
    return crank_talk_PrimitiveTables_lookupWithString(table, strlen(primitiveName), primitiveName);
}

CRANK_TALK_API crank_talk_PrimitiveFunction_t
crank_talk_PrimitiveTables_lookupWithString(crank_talk_PrimitivesTable_t* table, size_t primitiveNameSize, const char *primitiveName)
{
    intptr_t scanIndex = crank_talk_PrimitiveTables_scanFor(table, primitiveNameSize, primitiveName);
    if(scanIndex < 0)
        return NULL;

    return table->entries[scanIndex].primitive;
}
