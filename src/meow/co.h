#ifndef __BYTE_CODE_H
#define __BYTE_CODE_H

#include "../parser/node.h"

#include "mwobj.h"

typedef struct {
    u8* bytecode;
    u8* lnotab;
    u32 size;
    u32 capacity;
    MwObject** constants;
    MwObject** vars;
    int max_stack_size;
} MeoWMCodeObject;

typedef enum {
    OP_LOAD_CONST,
    OP_LOAD_VAR,
    OP_STORE_VAR,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_CMP,
    OP_JMP,
    OP_JE,
    OP_JNE,
    OP_JG,
    OP_JGE,
    OP_JL,
    OP_JLE,
    OP_OUT,
    OP_EXIT,
} MwOp;

typedef enum {
    MW_TYPE_NONE,
    MW_TYPE_INT,
    MW_TYPE_FLOAT,
    MW_TYPE_STR,
    MW_TYPE_ID,
} MwOperandType;

typedef union {
    int i;
    float f;
    StringView str;
} MwOperand;

typedef struct _MwInst {
    MwOp op;
    MwOperand operand;
    MwOperandType operand_type;
} MwInst;

MeoWMCodeObject* co_new(ASTNode** nodes);
void co_free(MeoWMCodeObject* code);
void co_disasm(MeoWMCodeObject* code);

int lnotab_get_line_from_ip(u8* lnotab, u32 ip);

#endif