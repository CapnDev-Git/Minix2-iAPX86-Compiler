#include "parser.h"

const char *regs_names[REG_SIZE] = {"ax", "bx", "cx", "dx",
                                    "bp", "sp", "si", "di"};

void print_AST(NodeAST **AST, size_t lenAST) {
  for (size_t i = 0; i < lenAST; i++) {
    NodeAST *node = AST[i];
    printf("Address: %lx\n", node->adr);
    printf("ASM: %s\n", node->ASM);
    printf("Instruction: %s\n", node->opC);
    if (node->spe != NULL)
      printf("Specifier: %s\n", node->spe);
    if (node->mOp != NULL)
      printf("Memory operand: %s\n", node->mOp);
    if (node->imm != NULL)
      printf("Immediate data: %lx\n", *(node->imm));
    if (node->reg != NULL)
      printf("Register: %s\n", regs_names[*(node->reg)]);
    printf("\n");
  }
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
    node->ASM = strdup(ASM[i]);
    node->opC = strdup(tokens[2]); // instruction always at index 2
    node->spe = NULL;
    node->mOp = NULL;
    node->imm = NULL;
    node->reg = NULL;

    // Lex the rest of the tokens
    for (size_t j = 3; j < n; j++) {
      if (!strcmp(tokens[j], "byte") || !strcmp(tokens[j], "short")) {
        // Specifier
        node->spe = strdup(tokens[j]);
      } else if (tokens[j][0] == '[') {
        // Memory operand
        node->mOp = strdup(tokens[j]);
      } else if (isdigit(tokens[j][0]) || tokens[j][0] == '-') {
        // Immediate data
        node->imm = malloc(sizeof(size_t));
        *(node->imm) = strtoul(strdup(tokens[j]), NULL, 16);
      } else {
        // Register
        node->reg = malloc(sizeof(size_t));
        *(node->reg) = get_index(regs_names, REG_SIZE, tokens[j]);
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

void free_AST(NodeAST **AST, size_t lenAST) {
  for (size_t i = 0; i < lenAST; i++) {
    NodeAST *node = AST[i];
    free(node->opC);
    if (node->spe != NULL)
      free(node->spe);
    if (node->mOp != NULL)
      free(node->mOp);
    if (node->imm != NULL)
      free(node->imm);
    if (node->reg != NULL)
      free(node->reg);
    free(node);
  }
  free(AST);
}