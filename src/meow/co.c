#include "co.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../utils/darray.h"
#include "../utils/hashmap.h"
#include "../utils/str.h"

#include "../lut.h"

#define MWCO_INITIAL_CAPACITY 128

#define JMP_PATCH_PLACEHOLDER 0xDEADBEEF
typedef struct {
    u32 offset;
    StringView label;
} JmpPatch;

static void lnotab_forward(MeoWMCodeObject* code, u8 offset, int line) {
    static int last_line = 0;
    darray_pushd(code->lnotab, offset);

    int line_delta = darray_size(code->lnotab) > 1 ? line - last_line : line;
    if (line_delta > 255) {
        darray_pushd(code->lnotab, 255);
        line_delta -= 255;

        while (line_delta > 255) {
            darray_pushd(code->lnotab, 0);
            darray_pushd(code->lnotab, 255);
            line_delta -= 255;
        }
    } else {
        darray_pushd(code->lnotab, line_delta);
    }

    last_line = line;
}

int lnotab_get_line_from_ip(u8* lnotab, u32 ip) {
    int offset = 0;
    int line = 1;
    int i = 0;
    while (offset < ip) {
        offset += lnotab[i];
        line += lnotab[i + 1];
        i += 2;
    }

    return line;
}

static void generate_op(MeoWMCodeObject* code, u8 op, int line) {
    lnotab_forward(code, 0x01, line);

    if (code->size + 1 >= code->capacity) {
        code->capacity *= 2;
        code->bytecode = (u8*)realloc(code->bytecode, code->capacity);
    }

    code->bytecode[code->size++] = op;
}

static void generate_opoperand(MeoWMCodeObject* code, u8 op, u64 operand, u8 operand_size, int line) {
    lnotab_forward(code, 0x01 + operand_size, line);

    if (code->size + 1 + operand_size >= code->capacity) {
        code->capacity *= 2;
        code->capacity += operand_size;
        code->bytecode = (u8*)realloc(code->bytecode, code->capacity);
    }

    code->bytecode[code->size++] = op;
    memcpy(code->bytecode + code->size, &operand, operand_size);
    code->size += operand_size;
}

static void calc_max_stack_size(MeoWMCodeObject* code) {
    int max_stack_size = 0;
    int stack_size = 0;
    u32 ip = 0;

    while (ip < code->size) {
        u8 op = code->bytecode[ip];
        switch (op) {
            case OP_STORE_VAR:
                // stack_size--;
                ip += 3;
                break;
            case OP_LOAD_VAR:
            case OP_LOAD_CONST:
                stack_size++;
                ip += 3;
                break;
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
                // stack_size--;
                ip += 1;
                break;
            case OP_CMP:
                // stack_size -= 2;
                ip += 1;
                break;
            case OP_JMP:
            case OP_JE:
            case OP_JNE:
            case OP_JG:
            case OP_JGE:
            case OP_JL:
            case OP_JLE:
                ip += 5;
                break;
            case OP_OUT:
                // stack_size--;
                ip++;
                break;
            case OP_EXIT:
                ip++;
        }

        if (stack_size > max_stack_size)
            max_stack_size = stack_size;
    }

    code->max_stack_size = max_stack_size;
}

MeoWMCodeObject* co_new(ASTNode** nodes) {
    MeoWMCodeObject* code = (MeoWMCodeObject*)malloc(sizeof(MeoWMCodeObject));
    code->lnotab = darray_new(u8);
    code->capacity = MWCO_INITIAL_CAPACITY;
    code->size = 0;
    code->bytecode = (u8*)malloc(code->capacity);
    code->constants = darray_new(MwObject*);
    code->vars = darray_new(MwObject*);

    HashMap* hm_vars = hashmap_new();
    HashMap* hm_labels = hashmap_new();

    JmpPatch* jmp_patches = (JmpPatch*)darray_new(JmpPatch);

    for (int i = 0; i < darray_size(nodes); i++) {
        ASTNode* node = nodes[i];
        switch (node->op) {
            case AST_STORE_VAR: {
                uintptr_t idx = 0;
                if (!hashmap_get(hm_vars, node->operand.str.data, node->operand.str.len, NULL)) {
                    idx = darray_size(code->vars);
                    hashmap_set(hm_vars, node->operand.str.data, node->operand.str.len, idx);
                    darray_pushd(code->vars, (MwObject*)NULL);
                } else {
                    hashmap_get(hm_vars, node->operand.str.data, node->operand.str.len, &idx);
                }

                generate_opoperand(code, node->op, idx, 2, node->line);
            } break;
            case AST_LOAD_VAR: {
                uintptr_t idx = 0;
                hashmap_get(hm_vars, node->operand.str.data, node->operand.str.len, &idx);
                generate_opoperand(code, node->op, idx, 2, node->line);
            } break;
            case AST_LOAD_CONST: {
                switch (node->operand_type) {
                    case TYPE_INT:
                        darray_pushd(code->constants, mwobj_new(OBJ_INT, (MwObjectValue){.i = node->operand.i}, 4));
                        break;
                    case TYPE_FLOAT:
                        darray_pushd(code->constants, mwobj_new(OBJ_FLOAT, (MwObjectValue){.f = node->operand.f}, 4));
                        break;
                    case TYPE_STR:
                        darray_pushd(code->constants, mwobj_new(OBJ_STR, (MwObjectValue){.str = {node->operand.str.data, node->operand.str.len}}, 4));
                        break;
                    default:
                        darray_pushd(code->vars, mwobj_new(OBJ_NONE, (MwObjectValue){.i = 0}, 4));
                }

                generate_opoperand(code, node->op, darray_size(code->constants) - 1, 2, node->line);
            } break;
            case AST_LABEL:
                if (!hashmap_get(hm_labels, node->operand.str.data, node->operand.str.len, NULL))
                    hashmap_set(hm_labels, node->operand.str.data, node->operand.str.len, code->size);
                break;
            case AST_JMP:
            case AST_JE:
            case AST_JNE:
            case AST_JG:
            case AST_JGE:
            case AST_JL:
            case AST_JLE:
                {
                    JmpPatch patch = {code->size + 1, node->operand.str};
                    darray_pushd(jmp_patches, patch);
                    generate_opoperand(code, node->op, JMP_PATCH_PLACEHOLDER, 4, node->line);
                } break;
            case AST_ADD:
            case AST_SUB:
            case AST_MUL:
            case AST_DIV:
            case AST_CMP:
            case AST_OUT:
            case AST_EXIT:
                generate_op(code, node->op, node->line);
                break;
        }
    }

    for (int i = 0; i < darray_size(jmp_patches); i++) {
        JmpPatch patch = jmp_patches[i];
        uintptr_t idx = 0;
        hashmap_get(hm_labels, patch.label.data, patch.label.len, &idx);
        memcpy(code->bytecode + patch.offset, &idx, 4);
    }

    calc_max_stack_size(code);

    hashmap_free(hm_vars);
    hashmap_free(hm_labels);

    darray_free(jmp_patches);

    for (int i = 0; i < darray_size(nodes); i++)
        free(nodes[i]);
    darray_free(nodes);

    return code;
}

void co_disasm(MeoWMCodeObject* code) {
    u32 ip = 0;
    while (ip < code->size) {
        u8 op = code->bytecode[ip];
        switch (op) {
            case OP_STORE_VAR:
            case OP_LOAD_VAR:
            case OP_LOAD_CONST:
                printf("%s(%d)\n", lut_op_to_str[op], *(u16*)(code->bytecode + ip + 1));
                ip += 3;
                break;
            case OP_JMP:
            case OP_JE:
            case OP_JNE:
            case OP_JG:
            case OP_JGE:
            case OP_JL:
            case OP_JLE:
                printf("%s(%d)\n", lut_op_to_str[op], *(u16*)(code->bytecode + ip + 1));
                ip += 5;
                break;
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
            case OP_CMP:
            case OP_OUT:
            case OP_EXIT:
                printf("%s\n", lut_op_to_str[op]);
                ip++;
                break;
            default:
                printf("\n");
                ip++;
                break;
        }
    }
}

void co_free(MeoWMCodeObject* code) {
    free(code->bytecode);

    darray_free(code->lnotab);
    for (int i = 0; i < darray_size(code->constants); i++)
        mwobj_decref(code->constants[i]);
    darray_free(code->constants);

    for (int i = 0; i < darray_size(code->vars); i++)
        mwobj_decref(code->vars[i]);
    darray_free(code->vars);
    
    free(code);
}