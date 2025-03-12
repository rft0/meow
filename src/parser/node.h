#ifndef __NODE_H
#define __NODE_H

#include "../utils/str.h"

#define NO_OPERAND (ASTNodeOperand){0}

typedef enum {
    AST_LOAD_CONST,
    AST_LOAD_VAR,
    AST_STORE_VAR,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_CMP,
    AST_JMP,
    AST_JE,
    AST_JNE,
    AST_JG,
    AST_JGE,
    AST_JL,
    AST_JLE,
    AST_OUT,
    AST_EXIT,
    AST_LABEL,
} ASTNodeOp;

typedef enum {
    TYPE_NONE,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STR,
    TYPE_ID,
} ASTNodeOperandType;

typedef union {
    int i;
    float f;
    StringView str;
} ASTNodeOperand;

typedef struct _ASTNode {
    int line;
    int col;
    ASTNodeOp op;
    ASTNodeOperand operand;
    ASTNodeOperandType operand_type;
} ASTNode;

ASTNode* node_new(ASTNodeOp op, ASTNodeOperand operand, ASTNodeOperandType type, int line, int col);
void node_dump(ASTNode* node);

#endif