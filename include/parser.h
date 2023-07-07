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

void get_node(NodeAST *node, char *line);
void print_node(NodeAST *node);

void print_AST(NodeAST **AST, size_t lenAST);
void free_AST(NodeAST **AST, size_t lenAST);