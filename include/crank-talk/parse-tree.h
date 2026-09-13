#ifndef CRANK_TALK_PARSE_TREE_H
#define CRANK_TALK_PARSE_TREE_H

#pragma once

#include "object-model.h"

typedef struct crank_talk_ParseTreeNode_s crank_talk_ParseTreeNode_t;
typedef struct crank_talk_ParseTreeParseErrorNode_s crank_talk_ParseTreeParseErrorNode_t;
typedef struct crank_talk_ParseTreeAssignmentNode_s crank_talk_ParseTreeAssignmentNode_t;
typedef struct crank_talk_ParseTreeIdentifierReferenceNode_s crank_talk_ParseTreeIdentifierReferenceNode_t;
typedef struct crank_talk_ParseTreePseudoVariableReferenceNode_s crank_talk_ParseTreePseudoVariableReferenceNode_t;
    typedef struct crank_talk_ParseTreeNilReferenceNode_s crank_talk_ParseTreeNilReferenceNode_t;
    typedef struct crank_talk_ParseTreeFalseReferenceNode_s crank_talk_ParseTreeFalseReferenceNode_t;
    typedef struct crank_talk_ParseTreeTrueReferenceNode_s crank_talk_ParseTreeTrueReferenceNode_t;
    typedef struct crank_talk_ParseTreeSelfReferenceNode_s crank_talk_ParseTreeSelfReferenceNode_t;
    typedef struct crank_talk_ParseTreeSuperReferenceNode_s crank_talk_ParseTreeSuperReferenceNode_t;
    typedef struct crank_talk_ParseTreeThisContextReferenceNode_s crank_talk_ParseTreeThisContextReferenceNode_t;

typedef struct crank_talk_ParseTreeArgumentDefinitionNode_s crank_talk_ParseTreeArgumentDefinitionNode_t;
typedef struct crank_talk_ParseTreeBlockNode_s crank_talk_ParseTreeBlockNode_t;
typedef struct crank_talk_ParseTreeLiteralNode_s crank_talk_ParseTreeLiteralNode_t;
typedef struct crank_talk_ParseTreeCascadedMessageNode_s crank_talk_ParseTreeCascadedMessageNode_t;
typedef struct crank_talk_ParseTreeMessageCascadeNode_s crank_talk_ParseTreeMessageCascadeNode_t;
typedef struct crank_talk_ParseTreeMessageSendNode_s crank_talk_ParseTreeMessageSendNode_t;
typedef struct crank_talk_ParseTreeSequenceNode_s crank_talk_ParseTreeSequenceNode_t;
typedef struct crank_talk_ParseTreeArrayNode_s crank_talk_ParseTreeArrayNode_t;
typedef struct crank_talk_ParseTreeReturnNode_s crank_talk_ParseTreeReturnNode_t;
typedef struct crank_talk_ParseTreeAddMethodNode_s crank_talk_ParseTreeAddMethodNode_t;
typedef struct crank_talk_ParseTreeMethodDefinitionNode_s crank_talk_ParseTreeMethodDefinitionNode_t;
typedef struct crank_talk_ParseTreePragmaNode_s crank_talk_ParseTreePragmaNode_t;
typedef struct crank_talk_ParseTreeVariableDefinitionNode_s crank_talk_ParseTreeVariableDefinitionNode_t;

typedef struct crank_talk_ParseTreeCullBlockNode_s crank_talk_ParseTreeCullBlockNode_t;
typedef struct crank_talk_ParseTreeIfSelectionNode_s crank_talk_ParseTreeIfSelectionNode_t;

struct crank_talk_ParseTreeNode_s
{
    crank_talk_Object_t super;
    crank_talk_SourcePosition_t *sourcePosition;
};

struct crank_talk_ParseTreeParseErrorNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_oop_t errorMessage;
};

struct crank_talk_ParseTreeAssignmentNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_ParseTreeNode_t *store;
    crank_talk_ParseTreeNode_t *value;
};

struct crank_talk_ParseTreeIdentifierReferenceNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_Symbol_t *symbol;
};

struct crank_talk_ParseTreePseudoVariableReferenceNode_s
{
    crank_talk_ParseTreeNode_t super;
};

struct crank_talk_ParseTreeNilReferenceNode_s
{
    crank_talk_ParseTreePseudoVariableReferenceNode_t super;
};

struct crank_talk_ParseTreeFalseReferenceNode_s
{
    crank_talk_ParseTreePseudoVariableReferenceNode_t super;
};

struct crank_talk_ParseTreeTrueReferenceNode_s
{
    crank_talk_ParseTreePseudoVariableReferenceNode_t super;
};

struct crank_talk_ParseTreeSelfReferenceNode_s
{
    crank_talk_ParseTreePseudoVariableReferenceNode_t super;
};

struct crank_talk_ParseTreeSuperReferenceNode_s
{
    crank_talk_ParseTreePseudoVariableReferenceNode_t super;
};

struct crank_talk_ParseTreeThisContextReferenceNode_s
{
    crank_talk_ParseTreePseudoVariableReferenceNode_t super;
};

struct crank_talk_ParseTreeArgumentDefinitionNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_Symbol_t *name;
    crank_talk_oop_t isSelf;
};

struct crank_talk_ParseTreeBlockNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_Array_t *arguments;
    crank_talk_ParseTreeNode_t *body;
};

struct crank_talk_ParseTreeLiteralNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_oop_t value;
};

struct crank_talk_ParseTreeCascadedMessageNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_ParseTreeNode_t *selector;
    crank_talk_Array_t *arguments;
};

struct crank_talk_ParseTreeMessageCascadeNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_ParseTreeNode_t *receiver;
    crank_talk_Array_t *cascadedMessages;
};

struct crank_talk_ParseTreeMessageSendNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_ParseTreeNode_t *receiver;
    crank_talk_ParseTreeNode_t *selector;
    crank_talk_Array_t *arguments;
};

struct crank_talk_ParseTreeSequenceNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_Array_t *pragmas;
    crank_talk_Array_t *localVariables;
    crank_talk_Array_t *expressions;
};

struct crank_talk_ParseTreeArrayNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_Array_t *elements;
};

struct crank_talk_ParseTreeReturnNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_ParseTreeNode_t *expression;
};

struct crank_talk_ParseTreeAddMethodNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_ParseTreeNode_t *owner;
    crank_talk_ParseTreeNode_t *method;
};

struct crank_talk_ParseTreeMethodDefinitionNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_Symbol_t *selector;
    crank_talk_Array_t *arguments;
    crank_talk_ParseTreeNode_t *body;
};

struct crank_talk_ParseTreePragmaNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_Symbol_t *selector;
    crank_talk_Array_t *arguments;
};

struct crank_talk_ParseTreeVariableDefinitionNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_Symbol_t *name;
};

struct crank_talk_ParseTreeCullBlockNode_s
{
    crank_talk_ParseTreeNode_t super;
    
    crank_talk_ParseTreeNode_t *blockExpression;
    crank_talk_Array_t *arguments;
};

struct crank_talk_ParseTreeIfSelectionNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_ParseTreeNode_t *condition;
    crank_talk_ParseTreeNode_t *trueExpression;
    crank_talk_ParseTreeNode_t *falseExpression;
};


CRANK_TALK_API bool crank_talk_isParseTreeNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeErrorNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeAssignmentNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeIdentifierReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreePseudoVariableReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);

CRANK_TALK_API bool crank_talk_isParseTreeNilReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeFalseReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeTrueReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeSelfReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeSuperReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeThisContextReferenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);

CRANK_TALK_API bool crank_talk_isParseTreeArgumentDefinitionNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeBlockNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeLiteralNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeCascadedMessageNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeMessageCascadeNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeMessageSendNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeSequenceNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeArrayNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeReturnNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeAddMethodNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeMethodDefinitionNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreePragmaNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);
CRANK_TALK_API bool crank_talk_isParseTreeVariableDefinitionNode(crank_talk_VMContext_t *context, crank_talk_oop_t node);

CRANK_TALK_API void crank_talk_ParseTree_traverseAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *node, crank_talk_oop_t errorList);
CRANK_TALK_API void crank_talk_ParseTree_traverseListAndCollectErrorsInto(crank_talk_VMContext_t *context, crank_talk_Array_t *list, crank_talk_oop_t errorList);
CRANK_TALK_API bool crank_talk_ParseTree_checkAndPrintErrors(crank_talk_VMContext_t *context, crank_talk_ParseTreeNode_t *node);

CRANK_TALK_API crank_talk_oop_t crank_talk_ParseTreeCascadedMessageNode_asMessageSendWithReceiver(crank_talk_VMContext_t *context, crank_talk_ParseTreeCascadedMessageNode_t *cascadedMessage, crank_talk_oop_t receiver);

#endif //CRANK_TALK_PARSE_TREE_H