#include "parser.h"

char *format_ASM(char **tokens, size_t n) {
  // Allocate memory for the formatted string
  char *formatted = malloc(100 * sizeof(char));
  if (formatted == NULL)
    errx(1, "Can't allocate memory for the formatted string!");

  // Copy the first two tokens
  strcpy(formatted, tokens[0]);
  strcat(formatted, tokens[1]);
  for (size_t i = 0; i < 13 - strlen(tokens[1]); i++)
    strcat(formatted, " ");

  // Copy the rest of the tokens
  for (size_t i = 2; i < n; i++) {
    strcat(formatted, tokens[i]);
    strcat(formatted, " ");
  }

  // Return the formatted string after removing the last space
  formatted[strlen(formatted) - 1] = '\0';
  return formatted;
}

int isHex(const char *str) {
  while (*str != '\0') {
    if (!isxdigit(*str))
      return 0;
    str++;
  }

  return 1;
}

void get_node(NodeAST *node, char *line) {
  size_t n;
  char **tokens;

  // Tokenize/lex the given ASM line
  tokens = lex(line, &n);
  if (tokens == NULL)
    errx(1, "Can't tokenize the given ASM line!");

  // Initialize the new node with the static fields
  node->addr = strtoul(strdup(tokens[0]), NULL, 16);
  node->ASM = strdup(format_ASM(tokens, n));
  node->opC = strdup(tokens[2]); // instruction always at index 2
  node->opLen = strlen(tokens[1]) / 2;

  // Parse the rest of the tokens
  for (size_t j = 3; j < n; j++) {
    if (!strcmp(tokens[j], "byte") || !strcmp(tokens[j], "short")) {
      /* SPECIFIER */
      node->spe = strdup(tokens[j]);
    } else if (tokens[j][0] == '[') {
      /* MEMORY OPERAND (no process here since may need dereferencing) */
      // Remove the separator
      remove_separator(tokens[j]);

      // Set it as the operand 1 if it's not already set, otherwise set it as
      // the operand 2
      if (node->op1 == NULL)
        node->op1 = strdup(tokens[j]);
      else
        node->op2 = strdup(tokens[j]);

      node->mOp = strdup(tokens[j]);
    } else if (isHex(tokens[j]) || tokens[j][0] == '-') {
      /* IMMEDIATE DATA */

      // Set it as the operand 1 if it's not already set, otherwise set it as
      // the operand 2
      if (node->op1 == NULL)
        node->op1 = strdup(tokens[j]);
      else
        node->op2 = strdup(tokens[j]);

      node->imm = malloc(sizeof(uint16_t));
      if (node->imm == NULL)
        errx(1, "Can't allocate memory for the immediate data!");

      *(node->imm) = (uint16_t)strtol(tokens[j], NULL, 16);
    } else {
      /* REGISTER(s) */

      // Remove the separator
      remove_separator(tokens[j]);

      // Set it as the operand 1 if it's not already set, otherwise set it as
      // the operand 2
      if (node->op1 == NULL)
        node->op1 = strdup(tokens[j]);
      else
        node->op2 = strdup(tokens[j]);

      // Reallocate memory for the registers array
      *(node->nreg) += 1;
      node->regs = realloc(node->regs, *(node->nreg) * sizeof(size_t *));
      if (node->regs == NULL)
        errx(1, "Can't reallocate memory for the registers array!");

      // Allocate memory for the register index
      node->regs[*(node->nreg) - 1] = malloc(sizeof(size_t));
      if (node->regs[*(node->nreg) - 1] == NULL)
        errx(1, "Can't allocate memory for the register index!");

      // Get the register index
      *(node->regs[*(node->nreg) - 1]) =
          get_index(registers, ALL_REG_SIZE, tokens[j]);
    }
  }

  // Free memory allocated for tokens
  free_2d(tokens, n);
}

void free_AST(NodeAST **AST, size_t lenAST) {
  NodeAST *node;

  for (size_t i = 0; i < lenAST; i++) {
    // Get the node
    node = AST[i];

    // Free the node's fields
    free(node->ASM);
    free(node->opC);

    if (node->spe != NULL)
      free(node->spe);

    if (node->mOp != NULL)
      free(node->mOp);

    if (node->imm != NULL)
      free(node->imm);

    if (node->regs != NULL) {
      for (size_t j = 0; j < *(node->nreg); j++)
        if (node->regs[j] != NULL)
          free(node->regs[j]);
      free(node->regs);
    }
    free(node->nreg);

    // Free the node
    free(node);
  }
  free(AST);
}