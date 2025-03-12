#ifndef __AST_H
#define __AST_H

#include "../utils/str.h"

#include "token.h"
#include "node.h"

ASTNode** parse(Token** tokens);

#endif