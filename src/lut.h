#ifndef __LUT_H
#define __LUT_H

#include "parser/token.h"
#include "parser/node.h"


extern const char* lut_token_to_str[];
extern const TokenType lut_inst_to_tok[];
extern const ASTNodeOp lut_token_to_op[];
extern const char* lut_op_to_str[];

#endif