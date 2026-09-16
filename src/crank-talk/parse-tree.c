#include "crank-talk/parse-tree.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include "crank-talk/ordered-collection.h"
#include "crank-talk/string.h"
#include <stdio.h>

CRANK_TALK_API bool
crank_talk_isParseTreeNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeErrorNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeParseErrorNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeAssignmentNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeAssignmentNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeIdentifierReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeIdentifierReferenceNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreePseudoVariableReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreePseudoVariableReferenceNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeNilReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeNilReferenceNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeFalseReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeFalseReferenceNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeTrueReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeTrueReferenceNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeSelfReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeSelfReferenceNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeSuperReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeSuperReferenceNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeThisContextReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeThisContextReferenceNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeArgumentDefinitionNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeArgumentDefinitionNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeBlockNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeBlockNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeLiteralNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeLiteralNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeCascadedMessageNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeCascadedMessageNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeMessageCascadeNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeMessageCascadeNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeMessageSendNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeMessageSendNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeSequenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeSequenceNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeArrayNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeArrayNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeReturnNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeReturnNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeAddMethodNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeAddMethodNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeMethodDefinitionNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeMethodDefinitionNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreePragmaNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreePragmaNodeClass);
}

CRANK_TALK_API bool
crank_talk_isParseTreeVariableDefinitionNode(crank_talk_VMContext_t *context, crank_talk_oop_t node)
{
    return crank_talk_oop_isKindOf(context, node, context->roots.ParseTreeVariableDefinitionNodeClass);
}

CRANK_TALK_API void
crank_talk_ParseTree_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *node, crank_talk_oop_t errorList)
{
    if(!node)
        return;

    crank_talk_send1(context, context->roots.traverseAndCollectErrorsIntoSymbol, (crank_talk_oop_t)node, errorList);

}

CRANK_TALK_API void
crank_talk_ParseTree_traverseListAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_Array_t *list, crank_talk_oop_t errorList)
{
    size_t listElementCount = crank_talk_object_getSizeInSlots((crank_talk_oop_t)list);
    for(size_t i = 0; i < listElementCount; ++i)
    {
        crank_talk_ParseTreeNode_t *element = (crank_talk_ParseTreeNode_t *)list->elements[i];
        crank_talk_ParseTree_traverseAndCollectErrorsInto(context, element, errorList);
    }
}

static void
crank_talk_ParseTree_printError(crank_talk_oop_t errorNodeValue)
{
    // TODO: Print the location.
    crank_talk_ParseTreeParseErrorNode_t *errorNode = (crank_talk_ParseTreeParseErrorNode_t*)errorNodeValue;
    fprintf(stderr, "Parse error: " CRANK_TALK_STRING_FORMAT "\n", CRANK_TALK_STRING_FORMAT_ARG(errorNode->errorMessage));
}

CRANK_TALK_API bool
crank_talk_ParseTree_checkAndPrintErrors(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *node)
{

    crank_talk_oop_t errorList = crank_talk_OrderedCollection_create(context);
    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node, errorList);
    size_t errorCount = crank_talk_OrderedCollection_getSize(errorList);
    for(size_t i = 1; i <= errorCount; ++i)
    {
        crank_talk_oop_t errorNode = crank_talk_OrderedCollection_at(context, errorList, i);
        crank_talk_ParseTree_printError(errorNode);
    }
        
    return errorCount == 0;
}

CRANK_TALK_API crank_talk_oop_t
crank_talk_ParseTreeCascadedMessageNode_asMessageSendWithReceiver(crank_talk_VMContext_t *context, crank_talk_ParseTreeCascadedMessageNode_t *cascadedMessage, crank_talk_oop_t receiver)
{
    crank_talk_oop_t messageSendNodeValue = CrankAllocateValuesObjectWithClassName(context, ParseTreeMessageSendNode);
    crank_talk_ParseTreeMessageSendNode_t *messageSendNode = (crank_talk_ParseTreeMessageSendNode_t *)messageSendNodeValue;
    messageSendNode->super.sourcePosition = cascadedMessage->super.sourcePosition;
    messageSendNode->receiver = (crank_talk_ParseTreeNode_t*)receiver;
    messageSendNode->selector = cascadedMessage->selector;
    messageSendNode->arguments = cascadedMessage->arguments;
    return messageSendNodeValue;
}

static crank_talk_oop_t
crank_talk_ParseTreeParseErrorNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_oop_t errorNode = arguments[0];
    crank_talk_oop_t errorList = arguments[1];
    
    crank_talk_OrderedCollection_add(context, errorList, errorNode);
    
    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeAssignmentNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeAssignmentNode_t *node = (crank_talk_ParseTreeAssignmentNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->store, errorList);
    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->value, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeIdentifierReferenceNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreePseudoVariableReference_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeArgumentDefinitionNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeBlockNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeBlockNode_t *node = (crank_talk_ParseTreeBlockNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->arguments, errorList);
    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->body, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeLiteralNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeCascadedMessageNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeCascadedMessageNode_t *node = (crank_talk_ParseTreeCascadedMessageNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->selector, errorList);
    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->arguments, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeMessageCascadeNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeMessageCascadeNode_t *node = (crank_talk_ParseTreeMessageCascadeNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->receiver, errorList);
    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->cascadedMessages, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeMessageSendNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeMessageSendNode_t *node = (crank_talk_ParseTreeMessageSendNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->receiver, errorList);
    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->selector, errorList);
    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->arguments, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeSequenceNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeSequenceNode_t *node = (crank_talk_ParseTreeSequenceNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->pragmas, errorList);
    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->localVariables, errorList);
    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->expressions, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeArrayNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeArrayNode_t *node = (crank_talk_ParseTreeArrayNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->elements, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeReturnNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeReturnNode_t *node = (crank_talk_ParseTreeReturnNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->expression, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeAddMethodNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeAddMethodNode_t *node = (crank_talk_ParseTreeAddMethodNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->owner, errorList);
    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->method, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeMethodDefinitionNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreeMethodDefinitionNode_t *node = (crank_talk_ParseTreeMethodDefinitionNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->arguments, errorList);
    crank_talk_ParseTree_traverseAndCollectErrorsInto(context, node->body, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreePragmaNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    crank_talk_ParseTreePragmaNode_t *node = (crank_talk_ParseTreePragmaNode_t*)arguments[0];
    crank_talk_oop_t errorList = arguments[1];

    crank_talk_ParseTree_traverseListAndCollectErrorsInto(context, node->arguments, errorList);

    return arguments[0];
}

static crank_talk_oop_t
crank_talk_ParseTreeVariableDefinitionNode_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_oop_t closure, size_t argumentCount, crank_talk_oop_t *arguments)
{
    (void)context;
    (void)closure;
    (void)argumentCount;
    assert(argumentCount >= 2);
    return arguments[0];
}

void crank_talk_registerParseTreePrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeParseErrorNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeParseErrorNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeAssignmentNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeAssignmentNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeIdentifierReferenceNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeIdentifierReferenceNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreePseudoVariableReferenceNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreePseudoVariableReference_traverseAndCollectErrorsInto);

    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeArgumentDefinitionNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeArgumentDefinitionNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeBlockNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeBlockNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeLiteralNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeLiteralNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeCascadedMessageNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeCascadedMessageNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageCascadeNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeMessageCascadeNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMessageSendNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeMessageSendNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeSequenceNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeSequenceNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeArrayNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeArrayNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeReturnNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeReturnNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeAddMethodNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeAddMethodNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeMethodDefinitionNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeMethodDefinitionNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreePragmaNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreePragmaNode_traverseAndCollectErrorsInto);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeVariableDefinitionNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeVariableDefinitionNode_traverseAndCollectErrorsInto);
}