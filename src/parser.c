#include "parser.h"

void print_AST(NodeAST **AST, size_t lenAST) {
  for (size_t i = 0; i < lenAST; i++) {
    NodeAST *node = AST[i];
    printf("Address: %lx\n", node->adr);
    printf("ASM: %s\n", node->ASM);
    printf("Instruction: %s\n", node->opC);

    if (node->op1 != NULL)
      printf("Opcode 1: %s\n", node->op1);
    if (node->op2 != NULL)
      printf("Opcode 2: %s\n", node->op2);

    if (node->spe != NULL)
      printf("Specifier: %s\n", node->spe);
    if (node->mOp != NULL)
      printf("Memory operand: %s\n", node->mOp);
    if (node->imm != NULL)
      printf("Immediate data: %x\n", *(node->imm));
    if (node->regs != NULL) {
      for (size_t j = 0; j < *(node->nreg); j++)
        if (node->regs[j] != NULL)
          printf("Register %zu: %s\n", j, registers[*(node->regs[j])]);
    }
    printf("\n");
  }
}

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

void build_AST(char **ASM, size_t len, NodeAST ***AST, size_t *lenAST) {
  char *line;
  char **tokens;
  size_t n;

  for (size_t i = 0; i < len; i++) {
    line = strdup(ASM[i]);
    tokens = lex(line, &n);
    if (tokens == NULL)
      errx(1, "Can't tokenize the given ASM line!");

    // Create a new NodeAST structure
    NodeAST *node = malloc(sizeof(NodeAST));
    if (node == NULL)
      errx(1, "Can't allocate memory for the NodeAST!");

    // Initialize the new node
    node->adr = strtoul(strdup(tokens[0]), NULL, 16);
    node->ASM = strdup(format_ASM(tokens, n));
    node->opC = strdup(tokens[2]); // instruction always at index 2

    // Initialize the operands fields
    node->op1 = NULL;
    node->op2 = NULL;

    // Initialize the other fields
    node->spe = NULL;
    node->mOp = NULL;
    node->imm = NULL;

    // Initialize the registers array
    node->nreg = malloc(sizeof(size_t));
    if (node->nreg == NULL)
      errx(1, "Can't allocate memory for the registers array length!");
    *(node->nreg) = 0;

    node->regs = malloc(sizeof(size_t *)); // array of size_t pointers
    if (node->regs == NULL)
      errx(1, "Can't allocate memory for the registers array!");
    *(node->regs) = NULL;

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
      } else if (isdigit(tokens[j][0]) || tokens[j][0] == '-') {
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
            get_index(registers, REG_SIZE, tokens[j]);
      }
    }

    // Add the instruction NodeAST to the AST
    (*AST)[(*lenAST)++] = node;

    // Free memory allocated for tokens
    free_2d(tokens, n);
  }

  // Print the AST
  // print_AST(*AST, *lenAST);
}

void print_node(NodeAST *node) {
  printf("Address: %lx\n", node->adr);
  printf("ASM: %s\n", node->ASM);
  printf("Instruction: %s\n", node->opC);
  if (node->op1 != NULL)
    printf("Opcode 1: %s\n", node->op1);
  if (node->op2 != NULL)
    printf("Opcode 2: %s\n", node->op2);

  if (node->spe != NULL)
    printf("Specifier: %s\n", node->spe);
  if (node->mOp != NULL)
    printf("Memory operand: %s\n", node->mOp);
  if (node->imm != NULL)
    printf("Immediate data: %x\n", *(node->imm));

  if (node->regs != NULL) {
    for (size_t j = 0; j < *(node->nreg); j++)
      if (node->regs[j] != NULL)
        printf("Register %zu: %s\n", j, registers[*(node->regs[j])]);
  }
  printf("\n");
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