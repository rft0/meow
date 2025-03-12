#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

#include "../utils/darray.h"

#include "../mwerror.h"
#include "../lut.h"


typedef struct _Parser {
    Token** tokens;
    Token* c;
    int index;
} Parser;

void parser_init(Parser* parser, Token** tokens) {
    parser->tokens = tokens;
    parser->index = 0;
    parser->c = tokens[0];
}

static void advance(Parser* p) {
    if (p->index + 1 < darray_size(p->tokens)) {
        p->index++;
        p->c = p->tokens[p->index];
    }
}

static void skiptillf(Parser* p) {
    while (p->c->type != TOKEN_LF && p->c->type != TOKEN_EOF)
        advance(p);
}

static ASTNode* parse_nyan(Parser* parser) {
    int line = parser->c->line;
    int col = parser->c->col;

    advance(parser);
    if (parser->c->type != TOKEN_IDENTIFIER) {
        mwerror_new(ERROR_PARSER, parser->c->line, parser->c->col, "Expected identifier");
        skiptillf(parser);
        return NULL;
    }

    StringView id = parser->c->value;

    advance(parser);
    if (parser->c->type != TOKEN_COLON) {
        mwerror_new(ERROR_PARSER, parser->c->line, parser->c->col, "Expected ':'");
        skiptillf(parser);
        return NULL;
    }

    return node_new(AST_LABEL, (ASTNodeOperand){.str = id}, TYPE_ID, line, col);
}

static ASTNode* parse_lsv(Parser* parser) {
    ASTNodeOp op = lut_token_to_op[parser->c->type];
    int line = parser->c->line;
    int col = parser->c->col;

    advance(parser);
    if (parser->c->type != TOKEN_IDENTIFIER) {
        mwerror_new(ERROR_PARSER, parser->c->line, parser->c->col, "Expected identifier");
        skiptillf(parser);
        return NULL;
    }

    return node_new(op, (ASTNodeOperand){.str = parser->c->value}, TYPE_ID, line, col);
}

static ASTNode* parse_lc(Parser* parser) {
    int line = parser->c->line;
    int col = parser->c->col;

    advance(parser);

    ASTNodeOperand operand;
    ASTNodeOperandType type;
    switch (parser->c->type) {
        case TOKEN_LIT_INTEGER:
            operand.i = atoi(parser->c->value.data);
            type = TYPE_INT;
            break;
        case TOKEN_LIT_FLOAT:
            operand.f = atof(parser->c->value.data);
            type = TYPE_FLOAT;
            break;
        case TOKEN_LIT_STRING:
            operand.str = parser->c->value;
            type = TYPE_STR;
            break;
        default:
            mwerror_new(ERROR_PARSER, parser->c->line, parser->c->col, "Expected literal");
            skiptillf(parser);
            return NULL;
    }

    return node_new(AST_LOAD_CONST, operand, type, line, col);
}

static ASTNode* parse_jmp(Parser* parser) {
    ASTNodeOp op = lut_token_to_op[parser->c->type];
    int line = parser->c->line;
    int col = parser->c->col;

    advance(parser);
    if (parser->c->type != TOKEN_IDENTIFIER) {
        mwerror_new(ERROR_PARSER, parser->c->line, parser->c->col, "Expected identifier");
        skiptillf(parser);
        return NULL;
    }

    return node_new(op, (ASTNodeOperand){.str = parser->c->value}, TYPE_ID, line, col);
}

static ASTNode* parse_stmt(Parser* parser) {
    ASTNode* res = NULL;
    switch (parser->c->type) {
        case TOKEN_KW_NYAN:
            res = parse_nyan(parser);
            advance(parser);
            break;
        case TOKEN_LOAD_VAR:
        case TOKEN_STORE_VAR:
            res = parse_lsv(parser);
            advance(parser);
            break;
        case TOKEN_LOAD_CONST:
            res = parse_lc(parser);
            advance(parser);
            break;
        case TOKEN_ADD:
        case TOKEN_SUB:
        case TOKEN_MUL:
        case TOKEN_DIV:
        case TOKEN_CMP:
        case TOKEN_OUT:
        case TOKEN_EXIT:
            res = node_new(lut_token_to_op[parser->c->type], NO_OPERAND, TYPE_NONE, parser->c->line, parser->c->col);
            advance(parser);
            break;
        case TOKEN_JMP:
        case TOKEN_JE:
        case TOKEN_JNE:
        case TOKEN_JG:
        case TOKEN_JGE:
        case TOKEN_JL:
        case TOKEN_JLE:
            res = parse_jmp(parser);
            advance(parser);
            break;
        case TOKEN_LIT_INTEGER:
        case TOKEN_LIT_FLOAT:
        case TOKEN_LIT_STRING:
            mwerror_new(ERROR_PARSER, parser->c->line, parser->c->col, "Unexpected literal '%.*s'", (int)parser->c->value.len, parser->c->value.data);
            skiptillf(parser);
            return NULL;
            break;
        case TOKEN_IDENTIFIER:
            mwerror_new(ERROR_PARSER, parser->c->line, parser->c->col, "Unexpected identifier '%.*s'", (int)parser->c->value.len, parser->c->value.data);
            skiptillf(parser);
            return NULL;
        case TOKEN_COLON:
            mwerror_new(ERROR_PARSER, parser->c->line, parser->c->col, "Unexpected ':'");
            skiptillf(parser);
            return NULL;
        case TOKEN_LF:
            advance(parser);
            return NULL;
        case TOKEN_EOF:
            return NULL;
    }

    if (res && parser->c->type != TOKEN_EOF && parser->c->type != TOKEN_LF) {
        mwerror_new(ERROR_PARSER, parser->c->line, parser->c->col, "Expected line feed");
        return NULL;
    }


    return res;
}

ASTNode** parse(Token** tokens) {
    Parser parser;
    parser_init(&parser, tokens);

    ASTNode** nodes = (ASTNode**)darray_new(ASTNode*);
    while(parser.c && parser.c->type != TOKEN_EOF) {
        ASTNode* node = parse_stmt(&parser);
        if (node)
            darray_push(nodes, node); 
    }

    for (int i = 0; i < darray_size(tokens); i++)
        free(tokens[i]);
    darray_free(tokens);

    return nodes;
}