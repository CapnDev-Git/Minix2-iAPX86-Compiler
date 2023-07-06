#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "lexer.h"
#include "prints.h"

void print_AST(NodeAST **AST, size_t lenAST);
void build_AST(char ASM[MEMORY_SIZE][256], size_t len, NodeAST ***AST,
               size_t *lenAST);
void free_AST(NodeAST **AST, size_t lenAST);
void print_node(NodeAST *node);