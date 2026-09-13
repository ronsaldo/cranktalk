#include "crank-talk/context.h"
#include "crank-talk/analysis-evaluation.h"
#include "crank-talk/analysis-build.h"
#include "crank-talk/array.h"
#include "crank-talk/behavior.h"
#include "crank-talk/bytecode.h"
#include "crank-talk/dictionary.h"
#include "crank-talk/environment.h"
#include "crank-talk/gc.h"
#include "crank-talk/hir.h"
#include "crank-talk/ordered-collection.h"
#include "crank-talk/parse-tree.h"
#include "crank-talk/set.h"
#include "crank-talk/string.h"
#include <stdarg.h>
#include <stdlib.h>

#define CrankTalkSlotDef(clazz, fieldName) #fieldName, offsetof(crank_talk_ ## clazz ##_t, fieldName)

void crank_talk_registerAnalysisAndBuildPrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerAnalysisAndEvaluationPrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerControlFlowPrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerEnvironmentPrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerFunctionalPrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerMacrosPrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerNumbersPrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerHIRPrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerObjectPrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerParseTreePrimitives(crank_talk_VMContext_t* context);
void crank_talk_registerStringPrimitives(crank_talk_VMContext_t* context);

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
crank_talk_context_setClassTypeMetadataVList(crank_talk_VMContext_t *context, crank_talk_oop_t classValue, const char *name, va_list valist)
{
    assert(!crank_talk_oop_isNil(classValue));
    crank_talk_Class_t *clazz = (crank_talk_Class_t*)classValue;
    // Set the name.
    if(name)
    {
        crank_talk_oop_t nameSymbol = crank_talk_Symbol_internWithCString(context, name);
        clazz->name = (crank_talk_Symbol_t*)nameSymbol;
        crank_talk_SystemEnvironment_atPut(context, context->roots.systemEnvironment, nameSymbol, classValue);
    }

    crank_talk_oop_t slots = crank_talk_OrderedCollection_create(context);
    
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

    clazz->super.super.slots = crank_talk_OrderedCollection_asArray(context, slots);

    crank_talk_oop_t metaclassValue = (crank_talk_oop_t)crank_talk_oop_getClass(context, classValue);
    assert(!crank_talk_oop_isNil(metaclassValue));
    crank_talk_Metaclass_t *metaClass = (crank_talk_Metaclass_t*)metaclassValue;
    if(crank_talk_oop_isNil(metaClass->super.super.slots))
        metaClass->super.super.slots = crank_talk_Array_create(context, 0);
}

static void
crank_talk_context_setClassTypeMetadata(crank_talk_VMContext_t *context, crank_talk_oop_t classValue, const char *name, ...)
{
    va_list valist;
    va_start(valist, name);
    crank_talk_context_setClassTypeMetadataVList(context, classValue, name, valist);
    va_end(valist);
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


static crank_talk_oop_t
crank_talk_context_createClassAndMetaclass(crank_talk_VMContext_t *context, const char *name, crank_talk_oop_t superclass, size_t instanceSize, crank_talk_GCObjectDataType_t dataType, ...)
{
    // Create the class object.
    crank_talk_oop_t classValue = crank_talk_context_createAnonymousClassAndMetaclass(context, superclass, instanceSize, dataType);
    
    // Set the slots.
    va_list valist;
    va_start(valist, dataType);
    crank_talk_context_setClassTypeMetadataVList(context, classValue, name, valist);
    va_end(valist);

    // Create the allSlots table.
    crank_talk_context_createClassTables(context, classValue);
    return classValue;
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
    context->roots.BoxedFloatClass =           crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.FloatClass, sizeof(crank_talk_BoxedFloat_t), CrankTalkGCObjectDataTypeBytes);
    context->roots.SmallFloatClass =           crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.FloatClass, sizeof(crank_talk_SmallFloat_t), CrankTalkGCObjectDataTypeImmediate);
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
    context->roots.VariableClass        = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,          sizeof(crank_talk_Variable_t),        CrankTalkGCObjectDataTypeOops);
    context->roots.LiteralVariableClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.VariableClass,        sizeof(crank_talk_LiteralVariable_t), CrankTalkGCObjectDataTypeOops);
    context->roots.GlobalVariableClass  = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.LiteralVariableClass, sizeof(crank_talk_GlobalVariable_t),  CrankTalkGCObjectDataTypeOops);
    context->roots.LocalVariableClass   = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.LiteralVariableClass, sizeof(crank_talk_LocalVariable_t),   CrankTalkGCObjectDataTypeOops);
    context->roots.HIRVariableClass     = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.VariableClass,        sizeof(crank_talk_HIRVariable_t),     CrankTalkGCObjectDataTypeOops);
    context->roots.SlotClass            = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.VariableClass,        sizeof(crank_talk_Slot_t),            CrankTalkGCObjectDataTypeOops);

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
    context->roots.CollectionClass          = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass,            sizeof(crank_talk_Collection_t),         CrankTalkGCObjectDataTypeOops);
    context->roots.HashedCollectionClass    = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.CollectionClass,        sizeof(crank_talk_HashedCollection_t),   CrankTalkGCObjectDataTypeOops);
    context->roots.DictionaryClass          = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.HashedCollectionClass,  sizeof(crank_talk_Dictionary_t),         CrankTalkGCObjectDataTypeOops);
    context->roots.IdentityDictionaryClass  = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.DictionaryClass,        sizeof(crank_talk_IdentityDictionary_t), CrankTalkGCObjectDataTypeOops);
    context->roots.SystemEnvironmentClass   = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.IdentityDictionaryClass, sizeof(crank_talk_SystemEnvironment_t), CrankTalkGCObjectDataTypeOops);
    context->roots.MethodDictionaryClass    = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.DictionaryClass,         sizeof(crank_talk_MethodDictionary_t),  CrankTalkGCObjectDataTypeOops);

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

    context->roots.ByteStreamClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_ByteStream_t), CrankTalkGCObjectDataTypeOops);
    context->roots.StringStreamClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_StringStream_t), CrankTalkGCObjectDataTypeOops);

    context->roots.ExternalAddressClass = crank_talk_context_createAnonymousClassAndMetaclass(context, context->roots.ObjectClass, sizeof(crank_talk_ExternalAddress_t), CrankTalkGCObjectDataTypeBytes);

    // System environment
    context->roots.systemEnvironment = crank_talk_SystemEnvironment_create(context);

    // Boolean values
    context->roots.falseValue = CrankAllocateValuesObjectWithClassName(context, False);
    context->roots.trueValue = CrankAllocateValuesObjectWithClassName(context, True);

    // Symbol set
    context->roots.internedSymbolSet = crank_talk_IdentitySet_create(context);

    // Symbols
    context->roots.doesNotUnderstandSymbol = crank_talk_Symbol_internWithCString(context, "doesNotUnderstand:");
    context->roots.asStringSymbol          = crank_talk_Symbol_internWithCString(context, "asString");
    context->roots.printStringSymbol       = crank_talk_Symbol_internWithCString(context, "printString");
    context->roots.printOnSymbol           = crank_talk_Symbol_internWithCString(context, "printOn:");
    context->roots.fullPrintOnSymbol       = crank_talk_Symbol_internWithCString(context, "fullPrintOn:");
    
    context->roots.traverseAndCollectErrorsIntoSymbol = crank_talk_Symbol_internWithCString(context, "traverseAndCollectErrorsInto:");
    context->roots.analyzeAndEvaluateWithSymbol = crank_talk_Symbol_internWithCString(context, "analyzeAndEvaluateWith:");
    context->roots.analyzeAndBuildWithSymbol = crank_talk_Symbol_internWithCString(context, "analyzeAndBuildWith:");

    context->roots.analyzeAndEvaluateIdentifierReferenceWithSymbol = crank_talk_Symbol_internWithCString(context, "analyzeAndEvaluateIdentifierReference:with:");
    context->roots.analyzeAndBuildIdentifierReferenceWithSymbol = crank_talk_Symbol_internWithCString(context, "analyzeAndBuildIdentifierReference:with:");
    context->roots.analyzeAndBuildAssignmentWithValueWithSymbol = crank_talk_Symbol_internWithCString(context, "analyzeAndBuildAssignment:withValue:with:");

    context->roots.lookupSymbolRecursively = crank_talk_Symbol_internWithCString(context, "lookupSymbolRecursively:");

    context->roots.hasResultValueSymbol = crank_talk_Symbol_internWithCString(context, "hasResultValue");
    context->roots.isTerminatorSymbol = crank_talk_Symbol_internWithCString(context, "isTerminator");
    context->roots.getBytecodeAssemblyOperandWithSymbol = crank_talk_Symbol_internWithCString(context, "getBytecodeAssemblyOperandWith:");
    context->roots.emitBytecodeAssemblyWithSymbol = crank_talk_Symbol_internWithCString(context, "emitBytecodeAssemblyWith:");

    context->roots.valueSymbol = crank_talk_Symbol_internWithCString(context, "value");

    // Message send macro dictionary.
    context->roots.messageSendMacroDictionary = crank_talk_IdentityDictionary_create(context);

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

    crank_talk_context_setClassTypeMetadata(context, context->roots.CompiledFunctionClass, "CompiledFunction",
        CrankTalkSlotDef(CompiledFunction, name),
        CrankTalkSlotDef(CompiledFunction, argumentCount),
        CrankTalkSlotDef(CompiledFunction, temporaryCount),
        CrankTalkSlotDef(CompiledFunction, nativeEntryPoint),
        CrankTalkSlotDef(CompiledFunction, bytecode),
        CrankTalkSlotDef(CompiledFunction, literalVector),
        CrankTalkSlotDef(CompiledFunction, isBlock),
        CrankTalkSlotDef(CompiledFunction, definitionEnvironment),
        CrankTalkSlotDef(CompiledFunction, definitionArguments),
        CrankTalkSlotDef(CompiledFunction, definitionBody),
        CrankTalkSlotDef(CompiledFunction, hirFunction),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.BlockClosureClass, "BlockClosure",
        CrankTalkSlotDef(BlockClosure, function),
        CrankTalkSlotDef(BlockClosure, captureVector),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.MessageClass, "Message",
        CrankTalkSlotDef(Message, selector),
        CrankTalkSlotDef(Message, arguments),
        CrankTalkSlotDef(Message, lookupClass),
        NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.MagnitudeClass, "Magnitude", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.CharacterClass, "Character", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.NumberClass, "Number", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.FloatClass, "Float", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.BoxedFloatClass, "BoxedFloat", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.SmallFloatClass, "SmallFloat", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.FractionClass, "Fraction",
        CrankTalkSlotDef(Fraction, numerator),
        CrankTalkSlotDef(Fraction, denominator),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.IntegerClass, "Integer", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.LargeIntegerClass, "LargeInteger", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.LargeNegativeIntegerClass, "LargeNegativeInteger", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.LargePositiveIntegerClass, "LargePositiveInteger", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.SmallIntegerClass, "SmallInteger", NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.UndefinedObjectClass, "UndefinedObject", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.BooleanClass, "Boolean", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.FalseClass, "False", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.TrueClass, "True", NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.TombstoneClass, "Tombstone", NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.VariableClass, "Variable",
        CrankTalkSlotDef(Variable, name),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.LiteralVariableClass, "LiteralVariable",
        CrankTalkSlotDef(LiteralVariable, value),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.GlobalVariableClass, "GlobalVariable", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.LocalVariableClass, "LocalVariable", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.HIRVariableClass, "HIRVariable",
        CrankTalkSlotDef(HIRVariable, value),
        NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.SlotClass, "Slot",
        CrankTalkSlotDef(Slot, index),
        NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.LookupKeyClass, "LookupKey",
        CrankTalkSlotDef(LookupKey, key),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.AssociationClass, "Association",
        CrankTalkSlotDef(Association, value),
        NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.SourceCodeClass, "SourceCode",
        CrankTalkSlotDef(SourceCode, text),
        CrankTalkSlotDef(SourceCode, directory),
        CrankTalkSlotDef(SourceCode, name),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.AbstractSourcePositionClass, "AbstractSourcePosition", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.EmptySourcePositionClass, "EmptySourcePosition", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.SourcePositionClass, "SourcePosition",
        CrankTalkSlotDef(SourcePosition, sourceCode),
        CrankTalkSlotDef(SourcePosition, startIndex),
        CrankTalkSlotDef(SourcePosition, endIndex),
        CrankTalkSlotDef(SourcePosition, startLine),
        CrankTalkSlotDef(SourcePosition, endLine),
        CrankTalkSlotDef(SourcePosition, startColumn),
        CrankTalkSlotDef(SourcePosition, endColumn),
        NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.CrankTalkTokenClass, "CrankTalkToken",
        CrankTalkSlotDef(CrankTalkToken, sourcePosition),
        CrankTalkSlotDef(CrankTalkToken, errorMessage),
        CrankTalkSlotDef(CrankTalkToken, kind),
        NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.CollectionClass, "Collection", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.HashedCollectionClass, "HashedCollection",
        CrankTalkSlotDef(HashedCollection, size),
        CrankTalkSlotDef(HashedCollection, storage),
        NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.DictionaryClass, "Dictionary", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.IdentityDictionaryClass, "IdentityDictionary", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.SystemEnvironmentClass, "SystemEnvironment", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.MethodDictionaryClass, "MethodDictionary", NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.SetClass, "Set", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.IdentitySetClass, "IdentitySet", NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.SequenceableCollectionClass, "SequenceableCollection", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.ArrayedCollectionClass, "ArrayedCollection", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.ArrayClass, "Array", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.ByteArrayClass, "ByteArray", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.StringClass, "String", NULL);
    crank_talk_context_setClassTypeMetadata(context, context->roots.SymbolClass, "Symbol", NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.OrderedCollectionClass, "OrderedCollection",
        CrankTalkSlotDef(OrderedCollection, size),
        CrankTalkSlotDef(OrderedCollection, storage),
        NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.ByteStreamClass, "ByteStream",
        CrankTalkSlotDef(ByteStream, size),
        CrankTalkSlotDef(ByteStream, storage),
        NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.StringStreamClass, "StringStream",
        CrankTalkSlotDef(StringStream, size),
        CrankTalkSlotDef(StringStream, storage),
        NULL);

    crank_talk_context_setClassTypeMetadata(context, context->roots.ExternalAddressClass, "ExternalAddress", NULL);

    // Tables
    crank_talk_context_createClassTables(context, context->roots.ProtoObjectClass);
    crank_talk_context_createClassTables(context, context->roots.ObjectClass);
    crank_talk_context_createClassTables(context, context->roots.BehaviorClass);
    crank_talk_context_createClassTables(context, context->roots.ClassDescriptionClass);
    crank_talk_context_createClassTables(context, context->roots.ClassClass);
    crank_talk_context_createClassTables(context, context->roots.MetaclassClass);

    crank_talk_context_createClassTables(context, context->roots.CompiledFunctionClass);
    crank_talk_context_createClassTables(context, context->roots.BlockClosureClass);
    crank_talk_context_createClassTables(context, context->roots.MessageClass);

    crank_talk_context_createClassTables(context, context->roots.MagnitudeClass);
    crank_talk_context_createClassTables(context, context->roots.CharacterClass);
    crank_talk_context_createClassTables(context, context->roots.NumberClass);
    crank_talk_context_createClassTables(context, context->roots.FloatClass);
    crank_talk_context_createClassTables(context, context->roots.BoxedFloatClass);
    crank_talk_context_createClassTables(context, context->roots.SmallFloatClass);
    crank_talk_context_createClassTables(context, context->roots.FractionClass);
    crank_talk_context_createClassTables(context, context->roots.IntegerClass);
    crank_talk_context_createClassTables(context, context->roots.LargeIntegerClass);
    crank_talk_context_createClassTables(context, context->roots.LargeNegativeIntegerClass);
    crank_talk_context_createClassTables(context, context->roots.LargePositiveIntegerClass);
    crank_talk_context_createClassTables(context, context->roots.SmallIntegerClass);

    crank_talk_context_createClassTables(context, context->roots.UndefinedObjectClass);
    crank_talk_context_createClassTables(context, context->roots.BooleanClass);
    crank_talk_context_createClassTables(context, context->roots.FalseClass);
    crank_talk_context_createClassTables(context, context->roots.TrueClass);

    crank_talk_context_createClassTables(context, context->roots.TombstoneClass);

    crank_talk_context_createClassTables(context, context->roots.VariableClass);
    crank_talk_context_createClassTables(context, context->roots.LiteralVariableClass);
    crank_talk_context_createClassTables(context, context->roots.GlobalVariableClass);
    crank_talk_context_createClassTables(context, context->roots.LocalVariableClass);
    crank_talk_context_createClassTables(context, context->roots.HIRVariableClass);
    crank_talk_context_createClassTables(context, context->roots.SlotClass);

    crank_talk_context_createClassTables(context, context->roots.LookupKeyClass);
    crank_talk_context_createClassTables(context, context->roots.AssociationClass);

    crank_talk_context_createClassTables(context, context->roots.SourceCodeClass);
    crank_talk_context_createClassTables(context, context->roots.AbstractSourcePositionClass);
    crank_talk_context_createClassTables(context, context->roots.EmptySourcePositionClass);
    crank_talk_context_createClassTables(context, context->roots.SourcePositionClass);

    crank_talk_context_createClassTables(context, context->roots.CrankTalkTokenClass);

    crank_talk_context_createClassTables(context, context->roots.CollectionClass);
    crank_talk_context_createClassTables(context, context->roots.HashedCollectionClass);
    crank_talk_context_createClassTables(context, context->roots.DictionaryClass);
    crank_talk_context_createClassTables(context, context->roots.IdentityDictionaryClass);
    crank_talk_context_createClassTables(context, context->roots.SystemEnvironmentClass);
    crank_talk_context_createClassTables(context, context->roots.MethodDictionaryClass);

    crank_talk_context_createClassTables(context, context->roots.SetClass);
    crank_talk_context_createClassTables(context, context->roots.IdentitySetClass);

    crank_talk_context_createClassTables(context, context->roots.SequenceableCollectionClass);
    crank_talk_context_createClassTables(context, context->roots.ArrayedCollectionClass);
    crank_talk_context_createClassTables(context, context->roots.ArrayClass);
    crank_talk_context_createClassTables(context, context->roots.ByteArrayClass);
    crank_talk_context_createClassTables(context, context->roots.StringClass);
    crank_talk_context_createClassTables(context, context->roots.SymbolClass);
    
    crank_talk_context_createClassTables(context, context->roots.OrderedCollectionClass);

    crank_talk_context_createClassTables(context, context->roots.ByteStreamClass);
    crank_talk_context_createClassTables(context, context->roots.StringStreamClass);

    crank_talk_context_createClassTables(context, context->roots.ExternalAddressClass);
}

static void 
crank_talk_context_createFrontEndDefinitions(crank_talk_VMContext_t* context)
{
    // Parse tree
    context->roots.ParseTreeNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeNode", context->roots.ObjectClass, sizeof(crank_talk_ParseTreeNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeNode, sourcePosition),
        NULL);

    context->roots.ParseTreeParseErrorNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeErrorNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeParseErrorNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeParseErrorNode, errorMessage),
        NULL);
    context->roots.ParseTreeAssignmentNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeAssignmentNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeAssignmentNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeAssignmentNode, store),
        CrankTalkSlotDef(ParseTreeAssignmentNode, value),
        NULL);
    context->roots.ParseTreeIdentifierReferenceNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeIdentifierReferenceNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeIdentifierReferenceNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeIdentifierReferenceNode, symbol),
        NULL);
    context->roots.ParseTreePseudoVariableReferenceNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreePseudoVariableReferenceNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreePseudoVariableReferenceNode_t), CrankTalkGCObjectDataTypeOops,
        NULL);

    context->roots.ParseTreeNilReferenceNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeNilReferenceNode", context->roots.ParseTreePseudoVariableReferenceNodeClass, sizeof(crank_talk_ParseTreeNilReferenceNode_t), CrankTalkGCObjectDataTypeOops,
        NULL);
    context->roots.ParseTreeFalseReferenceNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeFalseReferenceNode", context->roots.ParseTreePseudoVariableReferenceNodeClass, sizeof(crank_talk_ParseTreeFalseReferenceNode_t), CrankTalkGCObjectDataTypeOops,
        NULL);
    context->roots.ParseTreeTrueReferenceNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeTrueReferenceNode", context->roots.ParseTreePseudoVariableReferenceNodeClass, sizeof(crank_talk_ParseTreeTrueReferenceNode_t), CrankTalkGCObjectDataTypeOops,
        NULL);
    context->roots.ParseTreeSelfReferenceNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeSelfReferenceNode", context->roots.ParseTreePseudoVariableReferenceNodeClass, sizeof(crank_talk_ParseTreeSelfReferenceNode_t), CrankTalkGCObjectDataTypeOops,
        NULL);
    context->roots.ParseTreeSuperReferenceNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeSuperReferenceNode", context->roots.ParseTreePseudoVariableReferenceNodeClass, sizeof(crank_talk_ParseTreeSuperReferenceNode_t), CrankTalkGCObjectDataTypeOops,
        NULL);
    context->roots.ParseTreeThisContextReferenceNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeThisContextReferenceNode", context->roots.ParseTreePseudoVariableReferenceNodeClass, sizeof(crank_talk_ParseTreeThisContextReferenceNode_t), CrankTalkGCObjectDataTypeOops,
        NULL);

    context->roots.ParseTreeArgumentDefinitionNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeArgumentDefinitionNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeArgumentDefinitionNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeArgumentDefinitionNode, name),
        NULL);
    context->roots.ParseTreeBlockNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeBlockNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeBlockNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeBlockNode, arguments),
        CrankTalkSlotDef(ParseTreeBlockNode, body),
        NULL);

    context->roots.ParseTreeLiteralNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeLiteralNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeLiteralNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeLiteralNode, value),
        NULL);

    context->roots.ParseTreeCascadedMessageNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeCascadedMessageNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeCascadedMessageNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeCascadedMessageNode, selector),
        CrankTalkSlotDef(ParseTreeCascadedMessageNode, arguments),
        NULL);

    context->roots.ParseTreeMessageCascadeNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeMessageCascadeNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeMessageCascadeNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeMessageCascadeNode, receiver),
        CrankTalkSlotDef(ParseTreeMessageCascadeNode, cascadedMessages),
        NULL);

    context->roots.ParseTreeMessageSendNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeMessageSendNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeMessageSendNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeMessageSendNode, receiver),
        CrankTalkSlotDef(ParseTreeMessageSendNode, selector),
        CrankTalkSlotDef(ParseTreeMessageSendNode, arguments),
        NULL);

    context->roots.ParseTreeSequenceNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeSequenceNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeSequenceNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeSequenceNode, pragmas),
        CrankTalkSlotDef(ParseTreeSequenceNode, localVariables),
        CrankTalkSlotDef(ParseTreeSequenceNode, expressions),
        NULL);

    context->roots.ParseTreeArrayNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeArrayNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeArrayNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeArrayNode, elements),
        NULL);

    context->roots.ParseTreeReturnNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeReturnNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeReturnNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeReturnNode, expression),
        NULL);

    context->roots.ParseTreeAddMethodNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeAddMethodNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeAddMethodNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeAddMethodNode, owner),
        CrankTalkSlotDef(ParseTreeAddMethodNode, method),
        NULL);

    context->roots.ParseTreeMethodDefinitionNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeMethodDefinitionNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeMethodDefinitionNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeMethodDefinitionNode, selector),
        CrankTalkSlotDef(ParseTreeMethodDefinitionNode, arguments),
        CrankTalkSlotDef(ParseTreeMethodDefinitionNode, body),
        NULL);

    context->roots.ParseTreePragmaNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreePragmaNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreePragmaNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreePragmaNode, selector),
        CrankTalkSlotDef(ParseTreePragmaNode, arguments),
        NULL);

    context->roots.ParseTreeVariableDefinitionNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeVariableDefinitionNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeVariableDefinitionNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeVariableDefinitionNode, name),
        NULL);

    context->roots.ParseTreeCullBlockNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeCullBlockNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeCullBlockNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeCullBlockNode, blockExpression),
        CrankTalkSlotDef(ParseTreeCullBlockNode, arguments),
        NULL);
    context->roots.ParseTreeIfSelectionNodeClass = crank_talk_context_createClassAndMetaclass(context, "ParseTreeIfSelectionNode", context->roots.ParseTreeNodeClass, sizeof(crank_talk_ParseTreeIfSelectionNode_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(ParseTreeIfSelectionNode, condition),
        CrankTalkSlotDef(ParseTreeIfSelectionNode, trueExpression),
        CrankTalkSlotDef(ParseTreeIfSelectionNode, falseExpression),
        NULL);

    // Environment
    context->roots.AbstractEnvironmentClass = crank_talk_context_createClassAndMetaclass(context, "AbstractEnvironment", context->roots.ObjectClass, sizeof(crank_talk_AbstractEnvironment_t), CrankTalkGCObjectDataTypeOops, NULL);
    context->roots.EmptyEnvironmentClass = crank_talk_context_createClassAndMetaclass(context, "EmptyEnvironment", context->roots.AbstractEnvironmentClass, sizeof(crank_talk_EmptyEnvironment_t), CrankTalkGCObjectDataTypeOops, NULL);
    context->roots.SystemEnvironmentEnvironmentClass = crank_talk_context_createClassAndMetaclass(context, "SystemEnvironmentEnvironment", context->roots.AbstractEnvironmentClass, sizeof(crank_talk_SystemEnvironmentEnvironment_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(SystemEnvironmentEnvironment, parent),
        CrankTalkSlotDef(SystemEnvironmentEnvironment, systemEnvironment),
        NULL);
    context->roots.LexicalEnvironmentClass = crank_talk_context_createClassAndMetaclass(context, "LexicalEnvironment", context->roots.AbstractEnvironmentClass, sizeof(crank_talk_LexicalEnvironment_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(LexicalEnvironment, parent),
        CrankTalkSlotDef(LexicalEnvironment, symbolTable),
        NULL);
    context->roots.FunctionAnalysisEnvironmentClass = crank_talk_context_createClassAndMetaclass(context, "FunctionAnalysisEnvironment", context->roots.LexicalEnvironmentClass, sizeof(crank_talk_FunctionAnalysisEnvironment_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(FunctionAnalysisEnvironment, receiverValue),
        NULL);

    // HIR
    context->roots.HIRValueClass = crank_talk_context_createClassAndMetaclass(context, "HIRValue", context->roots.ObjectClass, sizeof(crank_talk_HIRValue_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRValue, sourcePosition),
        NULL);
    context->roots.HIRConstantValueClass = crank_talk_context_createClassAndMetaclass(context, "HIRConstantValue", context->roots.HIRValueClass, sizeof(crank_talk_HIRValue_t), CrankTalkGCObjectDataTypeOops, NULL);
    context->roots.HIRConstantLiteralValueClass = crank_talk_context_createClassAndMetaclass(context, "HIRConstantLiteralValue", context->roots.HIRConstantValueClass, sizeof(crank_talk_HIRConstantLiteralValue_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRConstantLiteralValue, value),
        NULL);
    context->roots.HIRConstantControlFlowEscapeValueClass = crank_talk_context_createClassAndMetaclass(context, "HIRConstantControlFlowEscapeValue", context->roots.HIRConstantValueClass, sizeof(crank_talk_HIRConstantControlFlowEscapeValue_t), CrankTalkGCObjectDataTypeOops, NULL);

    context->roots.HIRFunctionClass = crank_talk_context_createClassAndMetaclass(context, "HIRFunction", context->roots.HIRConstantValueClass, sizeof(crank_talk_HIRFunction_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRFunction, name),
        CrankTalkSlotDef(HIRFunction, arguments),
        CrankTalkSlotDef(HIRFunction, captures),
        CrankTalkSlotDef(HIRFunction, firstBasicBlock),
        CrankTalkSlotDef(HIRFunction, lastBasicBlock),
        CrankTalkSlotDef(HIRFunction, enumeratedLocalValues),
        NULL);

    context->roots.HIRFunctionLocalClass = crank_talk_context_createClassAndMetaclass(context, "HIRFunctionLocal", context->roots.HIRValueClass, sizeof(crank_talk_HIRFunctionLocal_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRFunctionLocal, name),
        CrankTalkSlotDef(HIRFunctionLocal, index),

        CrankTalkSlotDef(HIRFunctionLocal, bytecodeVectorIndex),
        NULL);
    context->roots.HIRArgumentClass = crank_talk_context_createClassAndMetaclass(context, "HIRArgument", context->roots.HIRFunctionLocalClass, sizeof(crank_talk_HIRArgument_t), CrankTalkGCObjectDataTypeOops, NULL);
    context->roots.HIRCaptureClass = crank_talk_context_createClassAndMetaclass(context, "HIRCapture", context->roots.HIRFunctionLocalClass, sizeof(crank_talk_HIRCapture_t), CrankTalkGCObjectDataTypeOops, NULL);
    context->roots.HIRBasicBlockClass = crank_talk_context_createClassAndMetaclass(context, "HIRBasicBlock", context->roots.HIRFunctionLocalClass, sizeof(crank_talk_HIRBasicBlock_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRBasicBlock, nextBlock),
        CrankTalkSlotDef(HIRBasicBlock, previousBlock),

        CrankTalkSlotDef(HIRBasicBlock, firstInstruction),
        CrankTalkSlotDef(HIRBasicBlock, lastInstruction),

        CrankTalkSlotDef(HIRBasicBlock, bytecodeLabel),
        NULL);

    context->roots.HIRInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRInstruction", context->roots.HIRFunctionLocalClass, sizeof(crank_talk_HIRInstruction_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRInstruction, nextInstruction),
        CrankTalkSlotDef(HIRInstruction, previousInstruction),
        NULL);

    context->roots.HIRAllocaInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRAllocaInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRAllocaInstruction_t), CrankTalkGCObjectDataTypeOops, NULL);
    context->roots.HIRBranchInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRBranchInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRBranchInstruction_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRBranchInstruction, destination),
        NULL);
    context->roots.HIRConditionalBranchInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRConditionalBranchInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRConditionalBranchInstruction_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRConditionalBranchInstruction, condition),
        CrankTalkSlotDef(HIRConditionalBranchInstruction, trueDestination),
        CrankTalkSlotDef(HIRConditionalBranchInstruction, falseDestination),
        NULL);
    context->roots.HIRPhiInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRPhiInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRPhiInstruction_t), CrankTalkGCObjectDataTypeOops,
        NULL);
    context->roots.HIRPhiSourceInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRPhiSourceInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRPhiSourceInstruction_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRPhiSourceInstruction, targetPhi),
        CrankTalkSlotDef(HIRPhiSourceInstruction, value),
        NULL);

    context->roots.HIRReturnInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRReturnInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRReturnInstruction_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRReturnInstruction, resultValue),
        NULL);
    context->roots.HIRSendInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRSendInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRSendInstruction_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRSendInstruction, selector),
        CrankTalkSlotDef(HIRSendInstruction, receiverWithArguments),
        NULL);
    context->roots.HIRUnreachableInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRUnreachableInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRUnreachableInstruction_t), CrankTalkGCObjectDataTypeOops, NULL);

    context->roots.HIRVariableLoadInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRVariableLoadInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRVariableLoadInstruction_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRVariableLoadInstruction, variable),
        NULL);
    context->roots.HIRVariableStoreInstructionClass = crank_talk_context_createClassAndMetaclass(context, "HIRVariableStoreInstruction", context->roots.HIRInstructionClass, sizeof(crank_talk_HIRVariableStoreInstruction_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRVariableStoreInstruction, variable),
        CrankTalkSlotDef(HIRVariableStoreInstruction, value),
        NULL);

    context->roots.HIRBuilderClass = crank_talk_context_createClassAndMetaclass(context, "HIRBuilder", context->roots.ObjectClass, sizeof(crank_talk_HIRBuilder_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(HIRBuilder, hirFunction),
        CrankTalkSlotDef(HIRBuilder, basicBlock),
        CrankTalkSlotDef(HIRBuilder, allocaBuilder),
        CrankTalkSlotDef(HIRBuilder, lexicalEnvironment),
        NULL);
        

    // Passes
    context->roots.AnalysisAndEvaluationContextClass = crank_talk_context_createClassAndMetaclass(context, "AnalysisAndEvaluationContext", context->roots.ObjectClass, sizeof(crank_talk_AnalysisAndEvaluationContext_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(AnalysisAndEvaluationContext, lexicalEnvironment),
        NULL);
    context->roots.AnalysisAndBuildContextClass = crank_talk_context_createClassAndMetaclass(context, "AnalysisAndBuildContext", context->roots.ObjectClass, sizeof(crank_talk_AnalysisAndBuildContext_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(AnalysisAndBuildContext, builder),
        NULL);

    // Bytecode assembler
    context->roots.BytecodeAssemblerClass = crank_talk_context_createClassAndMetaclass(context, "BytecodeAssembler", context->roots.ObjectClass, sizeof(crank_talk_AnalysisAndBuildContext_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(BytecodeAssembler, byteStream),
        CrankTalkSlotDef(BytecodeAssembler, literals),
        CrankTalkSlotDef(BytecodeAssembler, temporaryCount),
        CrankTalkSlotDef(BytecodeAssembler, currentSourcePosition),
        NULL);
    context->roots.BytecodeLabelClass = crank_talk_context_createClassAndMetaclass(context, "BytecodeAssembler", context->roots.ObjectClass, sizeof(crank_talk_AnalysisAndBuildContext_t), CrankTalkGCObjectDataTypeOops,
        CrankTalkSlotDef(BytecodeLabel, isDefined),
        CrankTalkSlotDef(BytecodeLabel, destinationValue),
        CrankTalkSlotDef(BytecodeLabel, pendingLocations),
        NULL);
}

static void 
crank_talk_registerPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_registerAnalysisAndBuildPrimitives(context);
    crank_talk_registerAnalysisAndEvaluationPrimitives(context);
    crank_talk_registerControlFlowPrimitives(context);
    crank_talk_registerEnvironmentPrimitives(context);
    crank_talk_registerFunctionalPrimitives(context);
    crank_talk_registerMacrosPrimitives(context);
    crank_talk_registerNumbersPrimitives(context);
    crank_talk_registerHIRPrimitives(context);
    crank_talk_registerObjectPrimitives(context);
    crank_talk_registerParseTreePrimitives(context);
    crank_talk_registerStringPrimitives(context);
}

CRANK_TALK_API crank_talk_VMContext_t*
crank_talk_vm_createContext(void)
{
    crank_talk_VMContext_t *context = calloc(1, sizeof(crank_talk_VMContext_t));
    crank_talk_heap_initialize(&context->heap);
    crank_talk_dynarray_initialize(&context->markingStack, sizeof(crank_talk_oop_t), 1<<17);
    crank_talk_context_createCoreDefinitions(context);
    crank_talk_context_createFrontEndDefinitions(context);
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
