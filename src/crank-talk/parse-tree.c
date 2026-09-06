#include "crank-talk/parse-tree.h"
#include "crank-talk/context.h"

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
