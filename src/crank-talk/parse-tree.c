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

void crank_talk_registerParseTreePrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.ParseTreeParseErrorNodeClass, "traverseAndCollectErrorsInto:", 2, crank_talk_ParseTreeParseErrorNode_traverseAndCollectErrorsInto);

    //typedef struct crank_talk_ParseTreeParseErrorNode_s crank_talk_ParseTreeParseErrorNode_t;
    //typedef struct crank_talk_ParseTreeAssignmentNode_s crank_talk_ParseTreeAssignmentNode_t;
    //typedef struct crank_talk_ParseTreeIdentifierReferenceNode_s crank_talk_ParseTreeIdentifierReferenceNode_t;
    //typedef struct crank_talk_ParseTreePseudoVariableReferenceNode_s crank_talk_ParseTreePseudoVariableReferenceNode_t;

    //typedef struct crank_talk_ParseTreeArgumentDefinitionNode_s crank_talk_ParseTreeArgumentDefinitionNode_t;
    //typedef struct crank_talk_ParseTreeBlockNode_s crank_talk_ParseTreeBlockNode_t;
    //typedef struct crank_talk_ParseTreeLiteralNode_s crank_talk_ParseTreeLiteralNode_t;
    //typedef struct crank_talk_ParseTreeCascadedMessageNode_s crank_talk_ParseTreeCascadedMessageNode_t;
    //typedef struct crank_talk_ParseTreeMessageCascadeNode_s crank_talk_ParseTreeMessageCascadeNode_t;
    //typedef struct crank_talk_ParseTreeMessageSendNode_s crank_talk_ParseTreeMessageSendNode_t;
    //typedef struct crank_talk_ParseTreeSequenceNode_s crank_talk_ParseTreeSequenceNode_t;
    //typedef struct crank_talk_ParseTreeArrayNode_s crank_talk_ParseTreeArrayNode_t;
    //typedef struct crank_talk_ParseTreeReturnNode_s crank_talk_ParseTreeReturnNode_t;
    //typedef struct crank_talk_ParseTreeAddMethodNode_s crank_talk_ParseTreeAddMethodNode_t;
    //typedef struct crank_talk_ParseTreeMethodDefinitionNode_s crank_talk_ParseTreeMethodDefinitionNode_t;
    //typedef struct crank_talk_ParseTreePragmaNode_s crank_talk_ParseTreePragmaNode_t;
    //typedef struct crank_talk_ParseTreeVariableDefinitionNode_s crank_talk_ParseTreeVariableDefinitionNode_t;
}