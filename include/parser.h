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

#define REG_SIZE 8

typedef struct {
  size_t adr;
  char *ASM; // to remove
  char *opC;
  char *spe;
  char *mOp;
  size_t *imm;
  size_t *reg;
} NodeAST;

void print_AST(NodeAST **AST, size_t lenAST);
void build_AST(char **ASM, size_t len, NodeAST ***AST, size_t *lenAST);
void free_AST(NodeAST **AST, size_t lenAST);