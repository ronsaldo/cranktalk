#ifndef CRANK_TALK_PRIMITIVES_H
#define CRANK_TALK_PRIMITIVES_H

#include "object-model.h"

struct crank_talk_PrimitiveContext_s
{
    crank_talk_VMContext_t *context;
    crank_talk_oop_t closure;
    size_t argumentCount;
    crank_talk_oop_t *arguments;
    crank_talk_oop_t resultValue;
    bool failed;
};

typedef struct crank_talk_PrimitiveTableEntry_s
{
    const char *name;
    crank_talk_PrimitiveFunction_t primitive;
} crank_talk_PrimitiveTableEntry_t;

typedef struct crank_talk_PrimitiveTables_s
{
    size_t capacity;
    size_t size;
    crank_talk_PrimitiveTableEntry_t *entries;
} crank_talk_PrimitivesTable_t;

CRANK_TALK_INLINE void
crank_talk_Primitive_fail(crank_talk_PrimitiveContext_t *primContext)
{
    primContext->failed = true;
}

CRANK_TALK_INLINE void
crank_talk_Primitive_return(crank_talk_PrimitiveContext_t *primContext, crank_talk_oop_t returnValue)
{
    primContext->resultValue = returnValue;
}


CRANK_TALK_API void crank_talk_PrimitiveTables_initialize(crank_talk_PrimitivesTable_t* table);
CRANK_TALK_API void crank_talk_PrimitiveTables_destroy(crank_talk_PrimitivesTable_t* table);
CRANK_TALK_API void crank_talk_PrimitiveTables_register(crank_talk_PrimitivesTable_t* table, const char *primitiveName, crank_talk_PrimitiveFunction_t primitiveFunction);
CRANK_TALK_API crank_talk_PrimitiveFunction_t crank_talk_PrimitiveTables_lookupWithCString(crank_talk_PrimitivesTable_t* table, const char *primitiveName);
CRANK_TALK_API crank_talk_PrimitiveFunction_t crank_talk_PrimitiveTables_lookupWithString(crank_talk_PrimitivesTable_t* table, size_t primitiveNameSize, const char *primitiveName);

#endif //CRANK_TALK_PRIMITIVES_H
