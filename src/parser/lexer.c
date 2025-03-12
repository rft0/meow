#include "lexer.h"

#include <string.h>
#include <ctype.h>

#include "../utils/darray.h"

#include "../mwerror.h"
#include "../lut.h"

typedef struct _Lexer {
    const char* src;
    char c;
    int pos;
    int line;
    int col;
} Lexer;

static void lexer_init(Lexer* lexer, const char* src) {
    lexer->src = src;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->c = src[0];
}

static void advance(Lexer* lexer) {
    if (lexer->c == '\n') {
        lexer->line++;
        lexer->col = 0;
    }

    lexer->col++;
    lexer->pos++;
    lexer->c = lexer->src[lexer->pos];
}

static void skipwhitespace(Lexer* lexer) {
    while(lexer->c != '\0' && isspace(lexer->c) && lexer->c != '\n')
        advance(lexer);
}

static void skipcomment(Lexer* lexer) {
    while (lexer->c && lexer->c != '\n')
        advance(lexer);
}
#include <stdio.h>
#include <stdlib.h>
static Token* get_identifier(Lexer* lexer) {
    StringView sv;
    sv.data = lexer->src + lexer->pos;
    sv.len = 0;

    while (isalnum(lexer->c) || lexer->c == '_' || lexer->c == '~') {
        sv.len++;
        advance(lexer);
    }

    if (sv.len == 4 && strncmp(sv.data, "nyan", sv.len) == 0)
        return token_new(TOKEN_KW_NYAN, sv, lexer->line, lexer->col - sv.len);

    if (sv.data[0] == 'm' && sv.data[sv.len - 1] == 'w' && sv.len > 3) {
        int i = 0, a = 0, b = 0, c = 0, d = 0;
        while (i < sv.len && sv.data[i] == 'm') {
            a++;
            i++;
        }

        if (i == 0 || a == 0) {
            mwerror_new(ERROR_LEXER, lexer->line, lexer->col - sv.len, "Invalid instruction");
            return NULL;
        }

        while (i < sv.len && sv.data[i] == 'e') {
            b++;
            i++;
        }
        if (b == 0) {
            mwerror_new(ERROR_LEXER, lexer->line, lexer->col - sv.len, "Invalid instruction");
            return NULL;
        }

        while (i < sv.len && sv.data[i] == 'o') {
            c++;
            i++;
        }

        if (c == 0) {
            mwerror_new(ERROR_LEXER, lexer->line, lexer->col - sv.len, "Invalid instruction");
            return NULL;
        }

        while (i < sv.len && sv.data[i] == 'w') {
            d++;
            i++;
        }

        if (d == 0) {
            mwerror_new(ERROR_LEXER, lexer->line, lexer->col - sv.len, "Invalid instruction");
            return NULL;
        }

        if ( i != sv.len) {
            mwerror_new(ERROR_LEXER, lexer->line, lexer->col - sv.len, "Invalid instruction");
            return NULL;
        }

        unsigned char inst = 0;
        unsigned char op_type = (a + b - 2) & 0b111;
        unsigned char op = (c + d - 2) & 0b1111;
        inst |= (op_type << 5);
        inst |= (op);

        return token_new(lut_inst_to_tok[inst], sv, lexer->line, lexer->col - sv.len);
    }

    return token_new(TOKEN_IDENTIFIER, sv, lexer->line, lexer->col - sv.len);
}

static Token* get_number(Lexer* lexer) {
    StringView sv;
    sv.data = lexer->src + lexer->pos;
    sv.len = 0;

    while (isdigit(lexer->c)) {
        sv.len++;
        advance(lexer);
    }

    if (lexer->c == '.') {
        sv.len++;
        advance(lexer);

        while (isdigit(lexer->c)) {
            sv.len++;
            advance(lexer);
        }

        return token_new(TOKEN_LIT_FLOAT, sv, lexer->line, lexer->col - sv.len);
    }

    return token_new(TOKEN_LIT_INTEGER, sv, lexer->line, lexer->col - sv.len);
}

static Token* get_string(Lexer* lexer) {
    advance(lexer);

    StringView sv;
    sv.data = lexer->src + lexer->pos;
    sv.len = 0;

    while (lexer->c != '"' && lexer->c != '\0' && lexer->c != '\n' && lexer->c != '\r') {
        sv.len++;
        advance(lexer);
    }

    if (lexer->c != '"') {
        mwerror_new(ERROR_LEXER, lexer->line, lexer->col, "Unterminated string");
        return token_new(TOKEN_EOF, EMPTY_STRV, lexer->line, lexer->col);
    }

    advance(lexer);
    return token_new(TOKEN_LIT_STRING, sv, lexer->line, lexer->col - sv.len - 1);
}

static Token* get_token(Lexer* lexer) {
    skipwhitespace(lexer);
    if (lexer->c == '\n') {
        int last_col = lexer->col;
        advance(lexer);
        return token_new(TOKEN_LF, EMPTY_STRV, lexer->line - 1, last_col);
    }


    if (!lexer->c)
        return token_new(TOKEN_EOF, EMPTY_STRV, lexer->line, lexer->col);

    if (isalpha(lexer->c) || lexer->c == '_' || lexer->c == '~')
        return get_identifier(lexer);
    else if (isdigit(lexer->c))
        return get_number(lexer);

    switch (lexer->c) {
        case '\n':
            advance(lexer);
            return token_new(TOKEN_LF, EMPTY_STRV, lexer->line - 1, lexer->col);
        case '#':
            skipcomment(lexer);
            if (lexer->c == '\n') {
                int last_col = lexer->col;
                advance(lexer);
                return token_new(TOKEN_LF, EMPTY_STRV, lexer->line - 1, last_col);
            }

            advance(lexer);
            return get_token(lexer);
        case ':':
            advance(lexer);
            return token_new(TOKEN_COLON, EMPTY_STRV, lexer->line, lexer->col);
        case '"':
            return get_string(lexer);
        default:
            mwerror_new(ERROR_LEXER, lexer->line, lexer->col, "Unexpected character '%c'", lexer->c);
            advance(lexer);
            return get_token(lexer);
    }
}

Token** lex(const char* src) {
    Lexer lexer;
    lexer_init(&lexer, src);

    Token** tokens = (Token**)darray_new(Token*);
    while (1) {
        Token* token = get_token(&lexer);
        if (!token)
            continue;

        if (token->type == TOKEN_EOF) {
            darray_push(tokens, token);
            break;
        }

        darray_push(tokens, token);
    }

    return tokens;
}