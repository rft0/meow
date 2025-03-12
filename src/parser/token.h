#ifndef __TOKEN_H
#define __TOKEN_H

#include "../utils/str.h"

typedef enum {
    TOKEN_EOF,
    TOKEN_LF,
    TOKEN_COLON,
    TOKEN_IDENTIFIER,
    TOKEN_LIT_INTEGER,
    TOKEN_LIT_FLOAT,
    TOKEN_LIT_STRING,
    TOKEN_KW_NYAN,
    // MEOW DECODED TOKENS:
    TOKEN_LOAD_CONST,
    TOKEN_LOAD_VAR,
    TOKEN_STORE_VAR,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_CMP,
    TOKEN_JMP,
    TOKEN_JE,
    TOKEN_JNE,
    TOKEN_JG,
    TOKEN_JGE,
    TOKEN_JL,
    TOKEN_JLE,
    TOKEN_OUT,
    TOKEN_EXIT,
} TokenType;

typedef struct {
    TokenType type;
    StringView value;
    int line;
    int col;
} Token;

Token* token_new(TokenType type, StringView value, int line, int col);
void token_dump(Token* token);

#endif