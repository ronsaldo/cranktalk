#ifndef CRANK_TALK_CONTEXT_H
#define CRANK_TALK_CONTEXT_H

#include "object-model.h"
#include "heap.h"
#include "dynarray.h"

#define CrankAllocateVariadicObjectWithClassName(context, objectClassName, dataType, variadicByteSize) \
    crank_talk_heap_allocateOopWithSizeAndType(&context->heap, sizeof(crank_talk_ ## objectClassName##_t) + variadicByteSize, dataType, context->roots.objectClassName##Class)

#define CrankAllocateValuesObjectWithClassName(context, objectClassName) CrankAllocateVariadicObjectWithClassName(context, objectClassName, CrankTalkGCObjectDataTypeOops, 0)

typedef struct crank_talk_VMContext_s crank_talk_VMContext_t;
typedef struct crank_talk_VMContextRoots_s crank_talk_VMContextRoots_t;

typedef struct crank_talk_VMContext_s crank_talk_VMContext_t;
typedef struct crank_talk_VMContextRoots_s crank_talk_VMContextRoots_t;

struct crank_talk_VMContextRoots_s
{
    crank_talk_oop_t ProtoObjectClass;
    crank_talk_oop_t ObjectClass;
    crank_talk_oop_t BehaviorClass;
    crank_talk_oop_t ClassDescriptionClass;
    crank_talk_oop_t ClassClass;
    crank_talk_oop_t MetaclassClass;

    crank_talk_oop_t CompiledFunctionClass;
    crank_talk_oop_t BlockClosureClass;
    crank_talk_oop_t MessageClass;

    crank_talk_oop_t MagnitudeClass;
    crank_talk_oop_t CharacterClass;
    crank_talk_oop_t NumberClass;
    crank_talk_oop_t FloatClass;
    crank_talk_oop_t BoxedFloatClass;
    crank_talk_oop_t SmallFloatClass;
    crank_talk_oop_t FractionClass;
    crank_talk_oop_t IntegerClass;
    crank_talk_oop_t LargeIntegerClass;
    crank_talk_oop_t LargeNegativeIntegerClass;
    crank_talk_oop_t LargePositiveIntegerClass;
    crank_talk_oop_t SmallIntegerClass;

    crank_talk_oop_t UndefinedObjectClass;
    crank_talk_oop_t BooleanClass;
    crank_talk_oop_t FalseClass;
    crank_talk_oop_t TrueClass;

    crank_talk_oop_t TombstoneClass;

    crank_talk_oop_t VariableClass;
    crank_talk_oop_t SlotClass;

    crank_talk_oop_t LookupKeyClass;
    crank_talk_oop_t AssociationClass;

    crank_talk_oop_t SourceCodeClass;
    crank_talk_oop_t AbstractSourcePositionClass;
    crank_talk_oop_t EmptySourcePositionClass;
    crank_talk_oop_t SourcePositionClass;

    crank_talk_oop_t CrankTalkTokenClass;

    crank_talk_oop_t CollectionClass;
    crank_talk_oop_t HashedCollectionClass;
    crank_talk_oop_t DictionaryClass;
    crank_talk_oop_t IdentityDictionaryClass;
    crank_talk_oop_t MethodDictionaryClass;

    crank_talk_oop_t SetClass;
    crank_talk_oop_t IdentitySetClass;

    crank_talk_oop_t SequenceableCollectionClass;
    crank_talk_oop_t ArrayedCollectionClass;
    crank_talk_oop_t ArrayClass;
    crank_talk_oop_t ByteArrayClass;
    crank_talk_oop_t StringClass;
    crank_talk_oop_t SymbolClass;
    
    crank_talk_oop_t OrderedCollectionClass;

    crank_talk_oop_t StringStreamClass;

    crank_talk_oop_t ExternalAddressClass;

    // Interned symbols
    crank_talk_oop_t internedSymbolSet;

    crank_talk_oop_t doesNotUnderstandSymbol;
    crank_talk_oop_t asStringSymbol;
    crank_talk_oop_t printStringSymbol;
    crank_talk_oop_t printOnSymbol;
    crank_talk_oop_t fullPrintOnSymbol;

};

struct crank_talk_VMContext_s
{
    crank_talk_VMContextRoots_t roots;
    crank_talk_heap_t heap;

    bool gcDisabled;
    crank_talk_dynarray_t markingStack;
};

CRANK_TALK_API crank_talk_VMContext_t *crank_talk_vm_createContext(void);
CRANK_TALK_API void crank_talk_vm_destroyContext(crank_talk_VMContext_t *context);


#endif //CRANK_TALK_CONTEXT_H