#include "analyse.h"

#include "../utils/darray.h"
#include "../utils/hashmap.h"

#include "../mwerror.h"

void analyse_ast(ASTNode** nodes) {
    HashMap* vars = hashmap_new();
    HashMap* labels = hashmap_new();

    for (int i = 0; i < darray_size(nodes); i++) {
        ASTNode* node = nodes[i];
        switch (node->op) {
            case AST_STORE_VAR:
                if (node->operand.str.len && !hashmap_get(vars, node->operand.str.data, node->operand.str.len, NULL))
                    hashmap_set(vars, node->operand.str.data, node->operand.str.len, 0);
                break;
            case AST_LOAD_VAR:
                if (node->operand.str.len && !hashmap_get(vars, node->operand.str.data, node->operand.str.len, NULL))
                    mwerror_new(ERROR_ANALYSE, node->line, node->col, "variable '%.*s' not defined", (int)node->operand.str.len, node->operand.str.data);
                break;
            case AST_LABEL:
                if (node->operand.str.len && hashmap_get(labels, node->operand.str.data, node->operand.str.len, NULL))
                    mwerror_new(ERROR_ANALYSE, node->line, node->col, "label '%.*s' already defined", (int)node->operand.str.len, node->operand.str.data);
                hashmap_set(labels, node->operand.str.data, node->operand.str.len, 0);
                break;
            default:
                break;
        }
    }

    for (int i = 0; i < darray_size(nodes); i++) {
        ASTNode* node = nodes[i];
        switch (node->op) {
            case AST_JMP:
            case AST_JE:
            case AST_JNE:
            case AST_JG:
            case AST_JGE:
            case AST_JL:
            case AST_JLE:
                if (!hashmap_get(labels, node->operand.str.data, node->operand.str.len, NULL))
                    mwerror_new(ERROR_ANALYSE, node->line, node->col, "label '%.*s' not defined", (int)node->operand.str.len, node->operand.str.data);
                break;
            default:
                break;
        }
    }


    hashmap_free(vars);
    hashmap_free(labels);
}