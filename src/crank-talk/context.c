#include "crank-talk/context.h"
#include <stdlib.h>

static crank_talk_oop_t
crank_talk_context_createAnonymousClassAndMetaclass(crank_talk_VMContext_t *context, crank_talk_oop_t superclass, size_t instanceSize, crank_talk_GCObjectDataType_t dataType)
{
    crank_talk_oop_t metaclassValue =  CrankAllocateValuesObjectWithClassName(context, Metaclass);
    crank_talk_Metaclass_t *metaclass = (crank_talk_Metaclass_t*)metaclassValue;
    if(!crank_talk_oop_isNil(superclass))
        metaclass->super.super.superclass = crank_talk_oop_getClass(context, superclass);

    metaclass->super.super.instanceSize      = crank_talk_oop_encodeSmallInteger(sizeof(crank_talk_Class_t));
    metaclass->super.super.instanceDataType  = crank_talk_oop_encodeSmallInteger(CrankTalkGCObjectDataTypeOops);

    crank_talk_oop_t classValue = crank_talk_heap_allocateOopWithSizeAndType(&context->heap, sizeof(crank_talk_Class_t), CrankTalkGCObjectDataTypeOops, metaclassValue);
    crank_talk_Class_t *clazz = (crank_talk_Class_t*)classValue;
    clazz->super.super.superclass = (crank_talk_Behavior_t*)superclass;
    clazz->super.super.instanceSize      = crank_talk_oop_encodeSmallInteger(instanceSize);
    clazz->super.super.instanceDataType  = crank_talk_oop_encodeSmallInteger(dataType);
    metaclass->thisClass = clazz;
    return classValue;
}

static void
crank_talk_context_fixupMetaclassType(crank_talk_VMContext_t *context, crank_talk_oop_t classToFixup)
{
    crank_talk_Behavior_t *metaclass = crank_talk_oop_getClass(context, classToFixup);
    crank_talk_object_setBehavior((crank_talk_oop_t)metaclass, (crank_talk_Behavior_t*)context->roots.MetaclassClass);
}

static void 
crank_talk_context_createCoreDefinitions(crank_talk_VMContext_t* context)
{
    context->roots.ProtoObjectClass = crank_talk_context_createAnonymousClassAndMetaclass(context, crank_talk_oop_nil(),            sizeof(crank_talk_ProtoObject_t), CrankTalkGCObjectDataTypeOops);
    context->roots.ObjectClass      = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ProtoObjectClass, sizeof(crank_talk_Object_t),      CrankTalkGCObjectDataTypeOops);

    context->roots.BehaviorClass    = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,      sizeof(crank_talk_Behavior_t),    CrankTalkGCObjectDataTypeOops);
    context->roots.ClassClass       = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.BehaviorClass,    sizeof(crank_talk_Class_t),       CrankTalkGCObjectDataTypeOops);
    context->roots.MetaclassClass   = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.BehaviorClass,    sizeof(crank_talk_Metaclass_t),   CrankTalkGCObjectDataTypeOops);

    //// Short circuit.
    {
        crank_talk_Behavior_t *protoObjectMetaclass = crank_talk_oop_getClass(context, context->roots.ProtoObjectClass);
        protoObjectMetaclass->superclass = (crank_talk_Behavior_t*)context->roots.ClassClass;
    }

    //// Fixup the meta class type.
    crank_talk_context_fixupMetaclassType(context, context->roots.ProtoObjectClass);
    crank_talk_context_fixupMetaclassType(context, context->roots.ObjectClass);

    crank_talk_context_fixupMetaclassType(context, context->roots.BehaviorClass);
    crank_talk_context_fixupMetaclassType(context, context->roots.ClassClass);
    crank_talk_context_fixupMetaclassType(context, context->roots.MetaclassClass);

    // Functions and family
    context->roots.CompiledFunctionClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_CompiledFunction_t), CrankTalkGCObjectDataTypeOops);
    context->roots.BlockClosureClass     = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_BlockClosure_t),     CrankTalkGCObjectDataTypeOops);
    context->roots.MessageClass          = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_Message_t),          CrankTalkGCObjectDataTypeOops);

    // Magnitudes and numbers
    context->roots.MagnitudeClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_Magnitude_t), CrankTalkGCObjectDataTypeOops);
    context->roots.CharacterClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.MagnitudeClass, sizeof(crank_talk_Character_t), CrankTalkGCObjectDataTypeOops);
    
    context->roots.NumberClass =               crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.MagnitudeClass, sizeof(crank_talk_Number_t), CrankTalkGCObjectDataTypeOops);
    context->roots.FloatClass =                crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.NumberClass, sizeof(crank_talk_Float_t), CrankTalkGCObjectDataTypeOops);
    context->roots.FractionClass =             crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.NumberClass, sizeof(crank_talk_Fraction_t), CrankTalkGCObjectDataTypeOops);
    context->roots.IntegerClass =              crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.NumberClass, sizeof(crank_talk_Integer_t), CrankTalkGCObjectDataTypeOops);
    context->roots.LargeIntegerClass =         crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.IntegerClass, sizeof(crank_talk_LargeInteger_t), CrankTalkGCObjectDataTypeOops);
    context->roots.LargeNegativeIntegerClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.LargeIntegerClass, sizeof(crank_talk_LargeNegativeInteger_t), CrankTalkGCObjectDataTypeOops);
    context->roots.LargePositiveIntegerClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.LargeIntegerClass, sizeof(crank_talk_LargePositiveInteger_t), CrankTalkGCObjectDataTypeOops);
    context->roots.SmallIntegerClass =         crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.IntegerClass, sizeof(crank_talk_SmallInteger_t), CrankTalkGCObjectDataTypeOops);

    context->roots.UndefinedObjectClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,  sizeof(crank_talk_UndefinedObject_t), CrankTalkGCObjectDataTypeImmediate);
    context->roots.BooleanClass         = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,  sizeof(crank_talk_Boolean_t),         CrankTalkGCObjectDataTypeOops);
    context->roots.FalseClass           = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.BooleanClass, sizeof(crank_talk_False_t),           CrankTalkGCObjectDataTypeOops);
    context->roots.TrueClass            = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.BooleanClass, sizeof(crank_talk_True_t),            CrankTalkGCObjectDataTypeOops);
    context->roots.TombstoneClass       = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,  sizeof(crank_talk_Tombstone_t),       CrankTalkGCObjectDataTypeOops);

    // Variables and slots
    context->roots.VariableClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,   sizeof(crank_talk_Variable_t), CrankTalkGCObjectDataTypeOops);
    context->roots.SlotClass     = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.VariableClass, sizeof(crank_talk_Slot_t),     CrankTalkGCObjectDataTypeOops);

    // Association
    context->roots.LookupKeyClass   = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_LookupKey_t),      CrankTalkGCObjectDataTypeOops);
    context->roots.AssociationClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.LookupKeyClass, sizeof(crank_talk_Association_t), CrankTalkGCObjectDataTypeOops);

    // Source code and position
    context->roots.SourceCodeClass             = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,                 sizeof(crank_talk_SourceCode_t),             CrankTalkGCObjectDataTypeOops);
    context->roots.AbstractSourcePositionClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,                 sizeof(crank_talk_AbstractSourcePosition_t), CrankTalkGCObjectDataTypeOops);
    context->roots.EmptySourcePositionClass    = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.AbstractSourcePositionClass, sizeof(crank_talk_EmptySourcePosition_t),    CrankTalkGCObjectDataTypeOops);
    context->roots.SourcePositionClass         = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.AbstractSourcePositionClass, sizeof(crank_talk_SourcePosition_t),         CrankTalkGCObjectDataTypeOops);

    // Token
    context->roots.CrankTalkTokenClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_CrankTalkToken_t), CrankTalkGCObjectDataTypeOops);

    // Hashed collections
    context->roots.CollectionClass          = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,           sizeof(crank_talk_Collection_t),          CrankTalkGCObjectDataTypeOops);
    context->roots.HashedCollectionClass    = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.CollectionClass,       sizeof(crank_talk_HashedCollection_t),    CrankTalkGCObjectDataTypeOops);
    context->roots.DictionaryClass          = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.HashedCollectionClass, sizeof(crank_talk_Dictionary_t),          CrankTalkGCObjectDataTypeOops);
    context->roots.IdentityDictionaryClass  = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.DictionaryClass,       sizeof(crank_talk_IdentityDictionary_t),  CrankTalkGCObjectDataTypeOops);
    context->roots.MethodDictionaryClass    = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.DictionaryClass,       sizeof(crank_talk_MethodDictionary_t),    CrankTalkGCObjectDataTypeOops);

    context->roots.SetClass         = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.HashedCollectionClass, sizeof(crank_talk_Set_t),         CrankTalkGCObjectDataTypeOops);
    context->roots.IdentitySetClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.SetClass,              sizeof(crank_talk_IdentitySet_t), CrankTalkGCObjectDataTypeOops);

    // Sequenceable collections
    context->roots.SequenceableCollectionClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.CollectionClass,             sizeof(crank_talk_SequenceableCollection_t), CrankTalkGCObjectDataTypeOops);
    context->roots.ArrayedCollectionClass      = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.SequenceableCollectionClass, sizeof(crank_talk_ArrayedCollection_t),      CrankTalkGCObjectDataTypeOops);
    context->roots.ArrayClass                  = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ArrayedCollectionClass,      sizeof(crank_talk_Array_t),                  CrankTalkGCObjectDataTypeOops);
    context->roots.ByteArrayClass              = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ArrayedCollectionClass,      sizeof(crank_talk_ByteArray_t),              CrankTalkGCObjectDataTypeBytes);
    context->roots.StringClass                 = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ArrayedCollectionClass,      sizeof(crank_talk_String_t),                 CrankTalkGCObjectDataTypeBytes);
    context->roots.SymbolClass                 = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ArrayedCollectionClass,      sizeof(crank_talk_Symbol_t),                 CrankTalkGCObjectDataTypeBytes);

    context->roots.OrderedCollectionClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.SequenceableCollectionClass,      sizeof(crank_talk_OrderedCollection_t),      CrankTalkGCObjectDataTypeOops);

    context->roots.ExternalAddressClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_ExternalAddress_t), CrankTalkGCObjectDataTypeBytes);
}

static void 
crank_talk_registerPrimitives(crank_talk_VMContext_t* context)
{
}

CRANK_TALK_API crank_talk_VMContext_t*
crank_talk_vm_createContext(void)
{
    crank_talk_VMContext_t *context = calloc(1, sizeof(crank_talk_VMContext_t));
    crank_talk_heap_initialize(&context->heap);
    crank_talk_dynarray_initialize(&context->markingStack, sizeof(crank_talk_oop_t), 1<<17);
    crank_talk_context_createCoreDefinitions(context);
    crank_talk_registerPrimitives(context);
    //crank_talk_gc_collect(context);
    return context;
}

CRANK_TALK_API void
crank_talk_vm_destroyContext(crank_talk_VMContext_t *context)
{
    crank_talk_dynarray_destroy(&context->markingStack);
    crank_talk_heap_freeAll(&context->heap);
    free(context);
}
