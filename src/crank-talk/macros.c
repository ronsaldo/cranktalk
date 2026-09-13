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

static crank_talk_ParseTreeNode_t *crank_talk_ParseTree_addEmptyCullBlockNode(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *blockExpression)
{
    crank_talk_ParseTreeCullBlockNode_t *cullBlock = (crank_talk_ParseTreeCullBlockNode_t *)CrankAllocateValuesObjectWithClassName(context, ParseTreeCullBlockNode);
    cullBlock->super.sourcePosition = blockExpression->sourcePosition;
    cullBlock->blockExpression = blockExpression;
    cullBlock->arguments = (crank_talk_Array_t*)crank_talk_Array_create(context, 0);
    return &cullBlock->super;
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

void crank_talk_registerMacrosPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetMessageMacro(context, "ifTrue:ifFalse:", crank_talk_MessageMacro_ifTrueIfFalse);
    crank_talk_createAndSetMessageMacro(context, "ifFalse:ifTrue:", crank_talk_MessageMacro_ifFalseIfTrue);
}
