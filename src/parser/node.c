#include "node.h"

#include <stdlib.h>
#include <stdio.h>

#include "../lut.h"

ASTNode* node_new(ASTNodeOp op, ASTNodeOperand operand, ASTNodeOperandType type, int line, int col) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->op = op;
    node->operand = operand;
    node->operand_type = type;
    node->line = line;
    node->col = col;
    return node;
}

void node_dump(ASTNode* node) {
    switch (node->op) {
        case AST_LOAD_CONST:
            switch (node->operand_type) {
                case TYPE_INT:
                    printf("LOAD_CONST(%d)\n", node->operand.i);
                    break;
                case TYPE_FLOAT:
                    printf("LOAD_CONST(%f)\n", node->operand.f);
                    break;
                case TYPE_STR:
                case TYPE_ID:
                    printf("LOAD_CONST(%.*s)\n", (int)node->operand.str.len, node->operand.str.data);
                    break;
                default:
                    printf("LOAD_CONST(NONE)\n");
                    break;
            }
            break;
        case AST_STORE_VAR:
        case AST_LOAD_VAR:
            printf("%s(%.*s)\n", lut_op_to_str[node->op], (int)node->operand.str.len, node->operand.str.data);
            break;
        case AST_ADD:
        case AST_SUB:
        case AST_MUL:
        case AST_DIV:
        case AST_CMP:
        case AST_OUT:
        case AST_EXIT:
            printf("%s\n", lut_op_to_str[node->op]);
            break;
        case AST_JMP:
        case AST_JE:
        case AST_JNE:
        case AST_JG:
        case AST_JGE:
        case AST_JL:
        case AST_JLE:
            printf("%s(%.*s)\n", lut_op_to_str[node->op], (int)node->operand.str.len, node->operand.str.data);
            break;
        case AST_LABEL:
            printf("LABEL(%.*s)\n", (int)node->operand.str.len, node->operand.str.data);
            break;
        default:
            break;
    }
}