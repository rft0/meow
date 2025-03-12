#include "lut.h"


// -- [ MeoWM Instruction Encoding ] --
// LOAD_CONST          V-Type (Variable manipulation operations)
// LOAD_VAR            V-Type
// STORE_VAR           V-Type
// ADD                 A-Type (Arithmetic operations)
// SUB                 A-Type
// MUL                 A-Type
// DIV                 A-Type
// CMP                 C-Type (Control flow operations)
// JMP                 C-Type
// JE                  C-Type
// JNE                 C-Type
// JG                  C-Type
// JGE                 C-Type
// JL                  C-Type
// JLE                 C-Type
// OUT                 S-Type (Operations that make system calls like printf, fopen etc.)

// -- [ Example ] --
// me           V Type
// mme          A Type
// meee         C Type
// mmme         C Type
// mmmee        S Type
// ow           2 - 2 = 0
// oooww        4 - 2 = 2
// oowwww       5 - 2 = 3


// First 3 bits represents the type of operation. (#m + #e - 1)
// Next 5 bits represents the operation itself. (#o + #w - 2)

// Address of instruction in memory.
// 0000000
// 0  3  5

// OP_LC       00000000
// OP_LV       00000001
// OP_SV       00000010

// OP_ADD      00100000
// OP_SUB      00100001
// OP_MUL      00100010
// OP_DIV      00100011

// OP_CMP      01000000
// OP_JMP      01000001
// OP_JE       01000010
// OP_JNE      01000011
// OP_JG       01000100
// OP_JGE      01000101
// OP_JL       01000110
// OP_JLE      01000111

// OP_OUT      01100000
// OP_EXIT     01100001

static const TokenType lut_inst_to_tok[] = {
    [0b00000000] = TOKEN_LOAD_CONST,   // OP_LC
    [0b00000001] = TOKEN_LOAD_VAR,     // OP_LV
    [0b00000010] = TOKEN_STORE_VAR,    // OP_SV
    
    [0b00100000] = TOKEN_ADD,          // OP_ADD
    [0b00100001] = TOKEN_SUB,          // OP_SUB
    [0b00100010] = TOKEN_MUL,          // OP_MUL
    [0b00100011] = TOKEN_DIV,          // OP_DIV
    
    [0b01000000] = TOKEN_CMP,          // OP_CMP
    [0b01000001] = TOKEN_JMP,          // OP_JMP
    [0b01000010] = TOKEN_JE,           // OP_JE
    [0b01000011] = TOKEN_JNE,          // OP_JNE
    [0b01000100] = TOKEN_JG,           // OP_JG
    [0b01000101] = TOKEN_JGE,          // OP_JGE
    [0b01000110] = TOKEN_JL,           // OP_JL
    [0b01000111] = TOKEN_JLE,          // OP_JLE
    
    [0b01100000] = TOKEN_OUT,          // OP_OUT
    [0b01100001] = TOKEN_EXIT,         // OP_EXIT
};

static const char* lut_token_to_str[] = {
    "EOF",
    "LF",
    "COLON",
    "IDENTIFIER",
    "LIT_INTEGER",
    "LIT_FLOAT",
    "LIT_STRING",
    "KW_NYAN",
    // MEOW DECODED TOKENS:
    "LOAD_CONST",
    "LOAD_VAR",
    "STORE_VAR",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "CMP",
    "JMP",
    "JE",
    "JNE",
    "JG",
    "JGE",
    "JL",
    "JLE",
    "OUT",
    "EXIT",
};

static const char* lut_op_to_str[] = {
    "LOAD_CONST",
    "LOAD_VAR",
    "STORE_VAR",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "CMP",
    "JMP",
    "JE",
    "JNE",
    "JG",
    "JGE",
    "JL",
    "JLE",
    "OUT",
    "EXIT",
};

static const ASTNodeOp lut_token_to_op[] = {
    [TOKEN_LOAD_CONST] = AST_LOAD_CONST,
    [TOKEN_LOAD_VAR] = AST_LOAD_VAR,
    [TOKEN_STORE_VAR] = AST_STORE_VAR,
    [TOKEN_ADD] = AST_ADD,
    [TOKEN_SUB] = AST_SUB,
    [TOKEN_MUL] = AST_MUL,
    [TOKEN_DIV] = AST_DIV,
    [TOKEN_CMP] = AST_CMP,
    [TOKEN_JMP] = AST_JMP,
    [TOKEN_JE] = AST_JE,
    [TOKEN_JNE] = AST_JNE,
    [TOKEN_JG] = AST_JG,
    [TOKEN_JGE] = AST_JGE,
    [TOKEN_JL] = AST_JL,
    [TOKEN_JLE] = AST_JLE,
    [TOKEN_OUT] = AST_OUT,
    [TOKEN_EXIT] = AST_EXIT,
};