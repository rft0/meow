#include "token.h"

#include <stdlib.h>
#include <stdio.h>

#include "../lut.h"

Token* token_new(TokenType type, StringView value, int line, int col) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->value = value;
    token->type = type;
    token->line = line;
    token->col = col;

    return token;
}

void token_dump(Token* token) {
    if (token->value.data)
        printf("Token(%s, %.*s, %d, %d)\n", lut_token_to_str[token->type], (int)token->value.len, token->value.data, token->line, token->col);
    else
        printf("Token(%s, %d, %d)\n", lut_token_to_str[token->type], token->line, token->col);
}