#include "crank-talk/context.h"
#include "crank-talk/array.h"
#include "crank-talk/dictionary.h"
#include "crank-talk/message.h"
#include "crank-talk/parse-tree.h"
#include "crank-talk/string.h"
#include <stdlib.h>

static void
crank_talk_createAndSetMessageMacro(crank_talk_VMContext_t *context, const char *messageSelector, crank_talk_CompiledFunctionNativeEntryPoint_t entryPoint)
{
    crank_talk_oop_t name = crank_talk_Symbol_internWithCString(context, messageSelector);
    crank_talk_oop_t compiledFunction = (crank_talk_oop_t)crank_talk_CompiledFunction_createForPrimitive(context, name, 1, entryPoint);
    crank_talk_IdentityDictionary_atPut(context, context->roots.messageSendMacroDictionary, name, compiledFunction);
}

static crank_talk_ParseTreeNode_t *
crank_talk_ParseTree_addEmptyCullBlockNode(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *blockExpression)
{
    crank_talk_ParseTreeCullBlockNode_t *cullBlock = (crank_talk_ParseTreeCullBlockNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeCullBlockNode);
    cullBlock->super.sourcePosition = blockExpression->sourcePosition;
    cullBlock->blockExpression = blockExpression;
    cullBlock->arguments = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    return &cullBlock->super;
}

static crank_talk_ParseTreeNode_t *
crank_talk_ParseTree_addNotMessage(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *expression)
{
    crank_talk_ParseTreeLiteralNode_t *selector = (crank_talk_ParseTreeLiteralNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    selector->super.sourcePosition = expression->sourcePosition;
    selector->value = context->roots.notSymbol;

    crank_talk_ParseTreeMessageSendNode_t *notSend = (crank_talk_ParseTreeMessageSendNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeMessageSendNode);
    notSend->super.sourcePosition = expression->sourcePosition;
    notSend->receiver = expression;
    notSend->selector = &selector->super;
    notSend->arguments = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    return &notSend->super;
}

static crank_talk_ParseTreeNode_t *
crank_talk_ParseTree_equalsNilMessage(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *expression)
{
    crank_talk_ParseTreeLiteralNode_t *selector = (crank_talk_ParseTreeLiteralNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    selector->super.sourcePosition = expression->sourcePosition;
    selector->value = context->roots.identityEqualsSymbol;

    crank_talk_ParseTreeLiteralNode_t *nilLiteral = (crank_talk_ParseTreeLiteralNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    nilLiteral->super.sourcePosition = expression->sourcePosition;
    nilLiteral->value = crank_talk_oop_nil();

    crank_talk_ParseTreeMessageSendNode_t *notSend = (crank_talk_ParseTreeMessageSendNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeMessageSendNode);
    notSend->super.sourcePosition = expression->sourcePosition;
    notSend->receiver = expression;
    notSend->selector = &selector->super;
    notSend->arguments = (crank_talk_Array_t*)crank_talk_Array_With(context, (crank_talk_oop_t)nilLiteral);
    return &notSend->super;
}

static crank_talk_ParseTreeNode_t *
crank_talk_ParseTree_equalsNotNilMessage(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *expression)
{
    crank_talk_ParseTreeLiteralNode_t *selector = (crank_talk_ParseTreeLiteralNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    selector->super.sourcePosition = expression->sourcePosition;
    selector->value = context->roots.identityNotEqualsSymbol;

    crank_talk_ParseTreeLiteralNode_t *nilLiteral = (crank_talk_ParseTreeLiteralNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    nilLiteral->super.sourcePosition = expression->sourcePosition;
    nilLiteral->value = crank_talk_oop_nil();

    crank_talk_ParseTreeMessageSendNode_t *notSend = (crank_talk_ParseTreeMessageSendNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeMessageSendNode);
    notSend->super.sourcePosition = expression->sourcePosition;
    notSend->receiver = expression;
    notSend->selector = &selector->super;
    notSend->arguments = (crank_talk_Array_t*)crank_talk_Array_With(context, (crank_talk_oop_t)nilLiteral);
    return &notSend->super;
}

static crank_talk_oop_t
crank_talk_MessageMacro_yourself(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    return (crank_talk_oop_t)sendNode->receiver;
}

static crank_talk_oop_t
crank_talk_MessageMacro_ifTrueIfFalse(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 2);

    crank_talk_oop_t ifSelectionNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeIfSelectionNode);
    crank_talk_ParseTreeIfSelectionNode_t *ifSelectionNode = (crank_talk_ParseTreeIfSelectionNode_t *)ifSelectionNodeValue;
    ifSelectionNode->super.sourcePosition = sendNode->super.sourcePosition;
    ifSelectionNode->condition = sendNode->receiver;
    ifSelectionNode->trueExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);
    ifSelectionNode->falseExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[1]);

    return ifSelectionNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_ifFalseIfTrue(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 2);

    crank_talk_oop_t ifSelectionNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeIfSelectionNode);
    crank_talk_ParseTreeIfSelectionNode_t *ifSelectionNode = (crank_talk_ParseTreeIfSelectionNode_t *)ifSelectionNodeValue;
    ifSelectionNode->super.sourcePosition = sendNode->super.sourcePosition;
    ifSelectionNode->condition = sendNode->receiver;
    ifSelectionNode->trueExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[1]);
    ifSelectionNode->falseExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);

    return ifSelectionNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_ifTrue(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_oop_t ifSelectionNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeIfSelectionNode);
    crank_talk_ParseTreeIfSelectionNode_t *ifSelectionNode = (crank_talk_ParseTreeIfSelectionNode_t *)ifSelectionNodeValue;
    ifSelectionNode->super.sourcePosition = sendNode->super.sourcePosition;
    ifSelectionNode->condition = sendNode->receiver;
    ifSelectionNode->trueExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);

    return ifSelectionNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_ifFalse(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_oop_t ifSelectionNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeIfSelectionNode);
    crank_talk_ParseTreeIfSelectionNode_t *ifSelectionNode = (crank_talk_ParseTreeIfSelectionNode_t *)ifSelectionNodeValue;
    ifSelectionNode->super.sourcePosition = sendNode->super.sourcePosition;
    ifSelectionNode->condition = sendNode->receiver;
    ifSelectionNode->falseExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);

    return ifSelectionNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_and(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_ParseTreeLiteralNode_t *falseLiteralNode = (crank_talk_ParseTreeLiteralNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    falseLiteralNode->super.sourcePosition = sendNode->super.sourcePosition;
    falseLiteralNode->value = context->roots.falseValue;

    crank_talk_oop_t ifSelectionNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeIfSelectionNode);
    crank_talk_ParseTreeIfSelectionNode_t *ifSelectionNode = (crank_talk_ParseTreeIfSelectionNode_t *)ifSelectionNodeValue;
    ifSelectionNode->super.sourcePosition = sendNode->super.sourcePosition;
    ifSelectionNode->condition = sendNode->receiver;
    ifSelectionNode->trueExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);
    ifSelectionNode->falseExpression = &falseLiteralNode->super;

    return ifSelectionNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_or(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_ParseTreeLiteralNode_t *trueLiteralNode = (crank_talk_ParseTreeLiteralNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeLiteralNode);
    trueLiteralNode->super.sourcePosition = sendNode->super.sourcePosition;
    trueLiteralNode->value = context->roots.trueValue;

    crank_talk_oop_t ifSelectionNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeIfSelectionNode);
    crank_talk_ParseTreeIfSelectionNode_t *ifSelectionNode = (crank_talk_ParseTreeIfSelectionNode_t *)ifSelectionNodeValue;
    ifSelectionNode->super.sourcePosition = sendNode->super.sourcePosition;
    ifSelectionNode->condition = sendNode->receiver;
    ifSelectionNode->trueExpression = &trueLiteralNode->super;
    ifSelectionNode->falseExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);

    return ifSelectionNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_whileTrue(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 0);

    crank_talk_oop_t whileNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeWhileNode);
    crank_talk_ParseTreeWhileNode_t *whileNode = (crank_talk_ParseTreeWhileNode_t *)whileNodeValue;
    whileNode->super.sourcePosition = sendNode->super.sourcePosition;
    whileNode->condition = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->receiver);
    return whileNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_whileTrueWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_oop_t whileNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeWhileNode);
    crank_talk_ParseTreeWhileNode_t *whileNode = (crank_talk_ParseTreeWhileNode_t *)whileNodeValue;
    whileNode->super.sourcePosition = sendNode->super.sourcePosition;
    whileNode->condition = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->receiver);
    whileNode->bodyExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);
    return whileNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_whileFalse(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 0);

    crank_talk_oop_t whileNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeWhileNode);
    crank_talk_ParseTreeWhileNode_t *whileNode = (crank_talk_ParseTreeWhileNode_t *)whileNodeValue;
    whileNode->super.sourcePosition = sendNode->super.sourcePosition;
    whileNode->condition = crank_talk_ParseTree_addNotMessage(context, crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->receiver));
    return whileNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_whileFalseWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_oop_t whileNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeWhileNode);
    crank_talk_ParseTreeWhileNode_t *whileNode = (crank_talk_ParseTreeWhileNode_t *)whileNodeValue;
    whileNode->super.sourcePosition = sendNode->super.sourcePosition;
    whileNode->condition = crank_talk_ParseTree_addNotMessage(context, crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->receiver));
    whileNode->bodyExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);
    return whileNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_whileNilWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_oop_t whileNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeWhileNode);
    crank_talk_ParseTreeWhileNode_t *whileNode = (crank_talk_ParseTreeWhileNode_t *)whileNodeValue;
    whileNode->super.sourcePosition = sendNode->super.sourcePosition;
    whileNode->condition = crank_talk_ParseTree_equalsNilMessage(context, crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->receiver));
    whileNode->bodyExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);
    return whileNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_whileNotNilWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_oop_t whileNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeWhileNode);
    crank_talk_ParseTreeWhileNode_t *whileNode = (crank_talk_ParseTreeWhileNode_t *)whileNodeValue;
    whileNode->super.sourcePosition = sendNode->super.sourcePosition;
    whileNode->condition = crank_talk_ParseTree_equalsNotNilMessage(context, crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->receiver));
    whileNode->bodyExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);
    return whileNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_doWhileTrueWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_oop_t whileNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeDoWhileNode);
    crank_talk_ParseTreeDoWhileNode_t *whileNode = (crank_talk_ParseTreeDoWhileNode_t *)whileNodeValue;
    whileNode->super.sourcePosition = sendNode->super.sourcePosition;
    whileNode->bodyExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->receiver);
    whileNode->condition = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]);
    return whileNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_doWhileFalseWith(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 1);

    crank_talk_oop_t whileNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeDoWhileNode);
    crank_talk_ParseTreeDoWhileNode_t *whileNode = (crank_talk_ParseTreeDoWhileNode_t *)whileNodeValue;
    whileNode->super.sourcePosition = sendNode->super.sourcePosition;
    whileNode->bodyExpression = crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->receiver);
    whileNode->condition = crank_talk_ParseTree_addNotMessage(context, crank_talk_ParseTree_addEmptyCullBlockNode(context, (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0]));
    return whileNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_toDo(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 2);

    crank_talk_oop_t toDoNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeToByDoNode);
    crank_talk_ParseTreeToByDoNode_t *toDoNode = (crank_talk_ParseTreeToByDoNode_t *)toDoNodeValue;
    toDoNode->super.sourcePosition = sendNode->super.sourcePosition;
    toDoNode->startExpression = sendNode->receiver;
    toDoNode->stopExpression = (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0];
    toDoNode->blockExpression = (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[1];
    return toDoNodeValue;
}

static crank_talk_oop_t
crank_talk_MessageMacro_toByDo(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount > 0);
    crank_talk_ParseTreeMessageSendNode_t *sendNode = (crank_talk_ParseTreeMessageSendNode_t *)arguments[0];
    assert(crank_talk_object_getSizeInSlots((crank_talk_oop_t)sendNode->arguments) == 3);

    crank_talk_oop_t toDoNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeToByDoNode);
    crank_talk_ParseTreeToByDoNode_t *toDoNode = (crank_talk_ParseTreeToByDoNode_t *)toDoNodeValue;
    toDoNode->super.sourcePosition = sendNode->super.sourcePosition;
    toDoNode->startExpression = sendNode->receiver;
    toDoNode->stopExpression = (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[0];
    toDoNode->stepExpression = (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[1];
    toDoNode->blockExpression = (crank_talk_ParseTreeNode_t*)sendNode->arguments->elements[2];
    return toDoNodeValue;
}

void crank_talk_registerMacrosPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetMessageMacro(context, "yourself", crank_talk_MessageMacro_yourself);
    crank_talk_createAndSetMessageMacro(context, "ifTrue:ifFalse:", crank_talk_MessageMacro_ifTrueIfFalse);
    crank_talk_createAndSetMessageMacro(context, "ifFalse:ifTrue:", crank_talk_MessageMacro_ifFalseIfTrue);
    crank_talk_createAndSetMessageMacro(context, "ifTrue:", crank_talk_MessageMacro_ifTrue);
    crank_talk_createAndSetMessageMacro(context, "ifFalse:", crank_talk_MessageMacro_ifFalse);

    crank_talk_createAndSetMessageMacro(context, "and:", crank_talk_MessageMacro_and);
    crank_talk_createAndSetMessageMacro(context, "or:", crank_talk_MessageMacro_or);

    crank_talk_createAndSetMessageMacro(context, "whileTrue", crank_talk_MessageMacro_whileTrue);
    crank_talk_createAndSetMessageMacro(context, "whileTrue:", crank_talk_MessageMacro_whileTrueWith);

    crank_talk_createAndSetMessageMacro(context, "whileFalse", crank_talk_MessageMacro_whileFalse);
    crank_talk_createAndSetMessageMacro(context, "whileFalse:", crank_talk_MessageMacro_whileFalseWith);

    crank_talk_createAndSetMessageMacro(context, "whileNil:", crank_talk_MessageMacro_whileNilWith);
    crank_talk_createAndSetMessageMacro(context, "whileNotNil:", crank_talk_MessageMacro_whileNotNilWith);

    crank_talk_createAndSetMessageMacro(context, "doWhileTrue:", crank_talk_MessageMacro_doWhileTrueWith);
    crank_talk_createAndSetMessageMacro(context, "doWhileFalse:", crank_talk_MessageMacro_doWhileFalseWith);

    crank_talk_createAndSetMessageMacro(context, "to:do:", crank_talk_MessageMacro_toDo);
    crank_talk_createAndSetMessageMacro(context, "to:by:do:", crank_talk_MessageMacro_toByDo);
}


