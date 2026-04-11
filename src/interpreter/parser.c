#include "parser.h"

static char *dup_string(const char *source) {
  size_t len = strlen(source) + 1;
  char *copy = malloc(len);
  if (copy == NULL)
    errx(1, "Can't allocate memory for string duplication!");

  memcpy(copy, source, len);
  return copy;
}

char *format_ASM(char **tokens, size_t n) {
  if (n < 3)
    errx(1, "Can't parse an ASM line with fewer than 3 tokens!");

  size_t size = strlen(tokens[0]) + strlen(tokens[1]) + 1;
  size_t padding = strlen(tokens[1]) < 13 ? 13 - strlen(tokens[1]) : 0;
  size += padding;

  for (size_t i = 2; i < n; i++)
    size += strlen(tokens[i]) + 1;

  // Allocate memory for the formatted string
  char *formatted = malloc(size);
  if (formatted == NULL)
    errx(1, "Can't allocate memory for the formatted string!");

  size_t offset = 0;
  offset +=
      snprintf(formatted + offset, size - offset, "%s%s", tokens[0], tokens[1]);
  for (size_t i = 0; i < padding; i++)
    formatted[offset++] = ' ';
  formatted[offset] = '\0';

  // Copy the rest of the tokens
  for (size_t i = 2; i < n; i++) {
    offset += snprintf(formatted + offset, size - offset, "%s%s", tokens[i],
                       i + 1 < n ? " " : "");
  }
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
  node->addr = strtoul(tokens[0], NULL, 16);
  char *formatted = format_ASM(tokens, n);
  NodeAST_finalize(node, formatted, strlen(tokens[1]) / 2);
  free(formatted);
  NodeAST_set_opcode(node, tokens[2]); // instruction always at index 2

  // Parse the rest of the tokens
  for (size_t j = 3; j < n; j++) {
    if (!strcmp(tokens[j], "byte") || !strcmp(tokens[j], "short")) {
      /* SPECIFIER */
      node->spe = dup_string(tokens[j]);
    } else if (tokens[j][0] == '[') {
      /* MEMORY OPERAND (no process here since may need dereferencing) */
      // Remove the separator
      remove_separator(tokens[j]);
      NodeAST_add_mem_operand(node, tokens[j], node->spe);
    } else if (isHex(tokens[j]) || tokens[j][0] == '-') {
      /* IMMEDIATE DATA */
      NodeAST_add_imm_operand(node, tokens[j],
                              (uint16_t)strtol(tokens[j], NULL, 16));
    } else {
      /* REGISTER(s) */

      // Remove the separator
      remove_separator(tokens[j]);
      NodeAST_add_reg_operand(node, tokens[j],
                              get_index(registers, ALL_REG_SIZE, tokens[j]));
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