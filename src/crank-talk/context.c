#include "crank-talk/context.h"
#include "crank-talk/array.h"
#include "crank-talk/behavior.h"
#include "crank-talk/gc.h"
#include "crank-talk/ordered-collection.h"
#include "crank-talk/set.h"
#include "crank-talk/string.h"
#include <stdarg.h>
#include <stdlib.h>

#define CrankTalkSlotDef(clazz, fieldName) #fieldName, offsetof(crank_talk_ ## clazz ##_t, fieldName)

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
crank_talk_context_setClassTypeMetadata(crank_talk_VMContext_t *context, crank_talk_oop_t classValue, const char *name, ...)
{
    assert(!crank_talk_oop_isNil(classValue));
    crank_talk_Class_t *clazz = (crank_talk_Class_t*)classValue;
    // Set the name.
    if(name)
    {
        crank_talk_oop_t nameSymbol = crank_talk_Symbol_internWithCString(context, name);
        clazz->name = (crank_talk_Symbol_t*)nameSymbol;
        // TODO: create the system environment
        //crank_talk_Namespace_setSymbolBinding(context, context->roots.globalNamespace, nameSymbol, classValue);
    }

    crank_talk_oop_t slots = crank_talk_OrderedCollection_create(context);
    
    va_list valist;
    va_start(valist, name);
    const char *fieldName;
    while((fieldName = va_arg(valist, const char *)) != NULL)
    {
        size_t offset = va_arg(valist, size_t);

        crank_talk_oop_t slotValue = CrankAllocateValuesObjectWithClassName(context, Slot);
        crank_talk_Slot_t *slot = (crank_talk_Slot_t*)slotValue;

        assert(offset >= sizeof(crank_talk_ObjectHeader_t));
        slot->super.name = (crank_talk_Symbol_t*)crank_talk_Symbol_internWithCString(context, fieldName);
        slot->index = crank_talk_oop_encodeSmallInteger((offset - sizeof(crank_talk_ObjectHeader_t)) / sizeof(crank_talk_oop_t));
        crank_talk_OrderedCollection_add(context, slots, slotValue);
    }
    va_end(valist);

    clazz->super.super.slots = crank_talk_OrderedCollection_asArray(context, slots);

    crank_talk_oop_t metaclassValue = (crank_talk_oop_t)crank_talk_oop_getClass(context, classValue);
    assert(!crank_talk_oop_isNil(metaclassValue));
    crank_talk_Metaclass_t *metaClass = (crank_talk_Metaclass_t*)metaclassValue;
    if(crank_talk_oop_isNil(metaClass->super.super.slots))
        metaClass->super.super.slots = crank_talk_Array_create(context, 0);
}

static void
crank_talk_context_createBehaviorSlotsTables(crank_talk_VMContext_t *context, crank_talk_oop_t behaviorValue)
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
        crank_talk_context_createBehaviorSlotsTables(context,superBehaviorValue);
        if(!crank_talk_oop_isNil(behavior->allSlots))
            return;

        allSlots = crank_talk_Array_concat(context, superBehavior->allSlots, behavior->slots);
    }
    behavior->allSlots = allSlots;

    crank_talk_Behavior_enumerateSlots(context, behaviorValue);
}

static void
crank_talk_context_createClassTables(crank_talk_VMContext_t *context, crank_talk_oop_t classValue)
{
    crank_talk_context_createBehaviorSlotsTables(context, classValue);
    crank_talk_oop_t metaclass = (crank_talk_oop_t)crank_talk_oop_getClass(context, classValue);
    crank_talk_context_createBehaviorSlotsTables(context, metaclass);
}


static void 
crank_talk_context_createCoreDefinitions(crank_talk_VMContext_t* context)
{
    context->roots.ProtoObjectClass = crank_talk_context_createAnonymousClassAndMetaclass(context, crank_talk_oop_nil(),            sizeof(crank_talk_ProtoObject_t), CrankTalkGCObjectDataTypeOops);
    context->roots.ObjectClass      = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ProtoObjectClass, sizeof(crank_talk_Object_t),      CrankTalkGCObjectDataTypeOops);

    context->roots.BehaviorClass         = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,      sizeof(crank_talk_Behavior_t),    CrankTalkGCObjectDataTypeOops);
    context->roots.ClassDescriptionClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.BehaviorClass,      sizeof(crank_talk_ClassDescription_t),    CrankTalkGCObjectDataTypeOops);
    context->roots.ClassClass            = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ClassDescriptionClass,    sizeof(crank_talk_Class_t),       CrankTalkGCObjectDataTypeOops);
    context->roots.MetaclassClass        = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ClassDescriptionClass,    sizeof(crank_talk_Metaclass_t),   CrankTalkGCObjectDataTypeOops);

    //// Short circuit.
    {
        crank_talk_Behavior_t *protoObjectMetaclass = crank_talk_oop_getClass(context, context->roots.ProtoObjectClass);
        protoObjectMetaclass->superclass = (crank_talk_Behavior_t*)context->roots.ClassClass;
    }

    //// Fixup the meta class type.
    crank_talk_context_fixupMetaclassType(context, context->roots.ProtoObjectClass);
    crank_talk_context_fixupMetaclassType(context, context->roots.ObjectClass);

    crank_talk_context_fixupMetaclassType(context, context->roots.BehaviorClass);
    crank_talk_context_fixupMetaclassType(context, context->roots.ClassDescriptionClass);
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

    // Symbol set
    context->roots.internedSymbolSet = crank_talk_IdentitySet_create(context);

    // Symbols
    context->roots.doesNotUnderstandSymbol = crank_talk_Symbol_internWithCString(context, "doesNotUnderstand:");
    context->roots.asStringSymbol          = crank_talk_Symbol_internWithCString(context, "asString");
    context->roots.printStringSymbol       = crank_talk_Symbol_internWithCString(context, "printString");
    context->roots.printOnSymbol           = crank_talk_Symbol_internWithCString(context, "printOn:");
    context->roots.fullPrintOnSymbol       = crank_talk_Symbol_internWithCString(context, "fullPrintOn:");

    // Field definitions
    crank_talk_context_setClassTypeMetadata(context, context->roots.ProtoObjectClass, "ProtoObject", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.ObjectClass, "Object", NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.BehaviorClass, "Behavior",
        CrankTalkSlotDef(Behavior, superclass),
        CrankTalkSlotDef(Behavior, methodDictionary),
        CrankTalkSlotDef(Behavior, instanceSize),
        CrankTalkSlotDef(Behavior, instanceDataType),
        CrankTalkSlotDef(Behavior, slots),
        CrankTalkSlotDef(Behavior, allSlots),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.ClassDescriptionClass, "ClassDescription", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.ClassClass, "Class",
        CrankTalkSlotDef(Class, name),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.MetaclassClass, "Metaclass",
        CrankTalkSlotDef(Metaclass, thisClass),
        NULL);

    // Tables
    crank_talk_context_createClassTables(context, context->roots.ProtoObjectClass);
    crank_talk_context_createClassTables(context, context->roots.ObjectClass);
    crank_talk_context_createClassTables(context, context->roots.BehaviorClass);
    crank_talk_context_createClassTables(context, context->roots.ClassDescriptionClass);
    crank_talk_context_createClassTables(context, context->roots.ClassClass);
    crank_talk_context_createClassTables(context, context->roots.MetaclassClass);

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
    crank_talk_gc_collect(context);
    return context;
}

CRANK_TALK_API void
crank_talk_vm_destroyContext(crank_talk_VMContext_t *context)
{
    crank_talk_dynarray_destroy(&context->markingStack);
    crank_talk_heap_freeAll(&context->heap);
    free(context);
}
