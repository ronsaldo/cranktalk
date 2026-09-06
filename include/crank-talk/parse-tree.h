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
    typedef struct crank_talk_ParseTreeLiteralCharacterNode_s crank_talk_ParseTreeLiteralCharacterNode_t;
    typedef struct crank_talk_ParseTreeLiteralFloatNode_s crank_talk_ParseTreeLiteralFloatNode_t;
    typedef struct crank_talk_ParseTreeLiteralIntegerNode_s crank_talk_ParseTreeLiteralIntegerNode_t;
    typedef struct crank_talk_ParseTreeLiteralStringNode_s crank_talk_ParseTreeLiteralStringNode_t;
    typedef struct crank_talk_ParseTreeLiteralSymbolNode_s crank_talk_ParseTreeLiteralSymbolNode_t;
    typedef struct crank_talk_ParseTreeLiteralValueNode_s crank_talk_ParseTreeLiteralValueNode_t;
typedef struct crank_talk_ParseTreeCascadedMessageNode_s crank_talk_ParseTreeCascadedMessageNode_t;
typedef struct crank_talk_ParseTreeMessageCascadeNode_s crank_talk_ParseTreeMessageCascadeNode_t;
typedef struct crank_talk_ParseTreeMessageSendNode_s crank_talk_ParseTreeMessageSendNode_t;
typedef struct crank_talk_ParseTreeSequenceNode_s crank_talk_ParseTreeSequenceNode_t;
typedef struct crank_talk_ParseTreeArrayNode_s crank_talk_ParseTreeArrayNode_t;
typedef struct crank_talk_ParseTreeReturnNode_s crank_talk_ParseTreeReturnNode_t;
typedef struct crank_talk_ParseTreeAddMethodNode_s crank_talk_ParseTreeAddMethodNode_t;
typedef struct crank_talk_ParseTreeMethodDefinitionNode_s crank_talk_ParseTreeMethodDefinitionNode_t;

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
};

struct crank_talk_ParseTreeLiteralCharacterNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_oop_t value;
};

struct crank_talk_ParseTreeLiteralFloatNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_oop_t value;
};

struct crank_talk_ParseTreeLiteralIntegerNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_oop_t value;
};

struct crank_talk_ParseTreeLiteralStringNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_oop_t value;
};

struct crank_talk_ParseTreeLiteralSymbolNode_s
{
    crank_talk_ParseTreeNode_t super;
    crank_talk_oop_t value;
};

struct crank_talk_ParseTreeLiteralValueNode_s
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

#endif //CRANK_TALK_PARSE_TREE_H