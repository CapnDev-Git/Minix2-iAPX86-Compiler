#include "globals.h"

// BOTH INTERPRETER AND DISASSEMBLER
size_t TEXT_SIZE = 0;
size_t DATA_SIZE = 0;
size_t TEXT_BEG = 0;
size_t DATA_BEG = 0;

// DISASSEMBLER
char ASM[MEMORY_SIZE][256] = {0};
size_t ASM_MAX_INDEX = 0;

const char *REG8[R8_SIZE] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char *REG16[R16_SIZE] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *SEGREG[SR_SIZE] = {"es", "cs", "ss", "ds"};

// GLOBAL FLAGS
int DISASSEMBLE = 0;
int DEBUG = 0;
int INTERPRET = 0;

// INTERPRETER
char INTPTR[MEMORY_SIZE][256] = {0};
size_t INTPTR_MAX_INDEX = 0;

const char *instructions[INSTR_SIZE] = {
    "adc", "add",  "and", "call", "cbw",  "cld", "cmp",        "cwd",
    "dec", "div",  "hlt", "in",   "inc",  "int", "jae",        "jb",
    "jbe", "je",   "jle", "jnb",  "jne",  "jnl", "jnle",       "jnbe",
    "jmp", "jl",   "lea", "loop", "mov",  "mul", "neg",        "or",
    "pop", "push", "rcl", "rep",  "ret",  "sar", "sbb",        "shl",
    "shr", "std",  "sub", "test", "xchg", "xor", "(undefined)"};

const char *registers[ALL_REG_SIZE] = {"ax", "bx", "cx", "dx", "sp", "bp",
                                       "si", "di", "al", "bl", "cl", "dl",
                                       "ah", "bh", "ch", "dh"};

const SyscallReturn syscallReturns[SYSCALLS_SIZE] = {
    {1, 0}, {4, 6}, {17, 0}, {54, -1}};

unsigned char text_mem[MEMORY_SIZE] = {0}; // reset the text memory
unsigned char data_mem[MEMORY_SIZE] = {0}; // reset the data memory
uint16_t regs[REG_SIZE] = {0};
int flags[FLAG_SIZE] = {0}; //{'-', '-', '-', '-'};
uint16_t *sp = &regs[SP];
uint16_t IP = 0;

void free_2d(char **array, size_t len) {
  for (size_t i = 0; i < len; i++)
    free(array[i]);
  free(array);
}

size_t get_index(const char **array, size_t size, const char *element) {
  for (size_t i = 0; i < size; i++) {
    if (!strcmp(array[i], element))
      return i;
  }
  return -1; // Element not found
}

void vector_init(Vector *vector) {
  vector->data = NULL;
  vector->size = 0;
  vector->capacity = 0;
}

void vector_pushback(Vector *vector, void *element) {
  if (vector->size >= vector->capacity) {
    int ncapa = (vector->capacity == 0) ? 1 : vector->capacity * 2;
    void **ndata = (void **)realloc(vector->data, ncapa * sizeof(void *));
    if (ndata == NULL)
      errx(1, "Memory allocation failed.\n");

    vector->data = ndata;
    vector->capacity = ncapa;
  }

  vector->data[vector->size] = element;
  vector->size++;
}

void vector_free(Vector *vector) {
  free(vector->data);
  vector->data = NULL;
  vector->size = 0;
  vector->capacity = 0;
}

void NodeAST_init(NodeAST *node) {
  // Initialize the new node
  node->addr = 0;
  node->ASM = NULL;
  node->opC = NULL;
  node->opLen = 0;

  // Initialize the other fields
  node->spe = NULL;
  node->mOp = NULL;
  node->imm = NULL;
  node->operandCount = 0;
  node->operandKinds[0] = OPERAND_NONE;
  node->operandKinds[1] = OPERAND_NONE;

  // Initialize the registers array
  node->nreg = malloc(sizeof(size_t));
  if (node->nreg == NULL)
    errx(1, "Can't allocate memory for the registers array length!");
  *(node->nreg) = 0;

  node->regs = malloc(sizeof(size_t *));
  if (node->regs == NULL)
    errx(1, "Can't allocate memory for the registers array!");
  *(node->regs) = NULL;
}

void NodeAST_free(NodeAST *node) {
  free(node->ASM);
  free(node->opC);
  free(node->spe);
  free(node->mOp);
  free(node->imm);

  if (node->regs != NULL) {
    for (size_t j = 0; j < *(node->nreg); j++)
      free(node->regs[j]);
    free(node->regs);
  }

  free(node->nreg);
  node->ASM = NULL;
  node->opC = NULL;
  node->spe = NULL;
  node->mOp = NULL;
  node->imm = NULL;
  node->operandCount = 0;
  node->operandKinds[0] = OPERAND_NONE;
  node->operandKinds[1] = OPERAND_NONE;
  node->regs = NULL;
  node->nreg = NULL;
}

static char *dup_string(const char *source) {
  size_t len = strlen(source) + 1;
  char *copy = malloc(len);
  if (copy == NULL)
    errx(1, "Can't allocate memory for string duplication!");

  memcpy(copy, source, len);
  return copy;
}

static void node_add_operand_kind(NodeAST *node, OperandKind kind) {
  if (node->operandCount < 2)
    node->operandKinds[node->operandCount] = kind;
  node->operandCount++;
}

void NodeAST_set_opcode(NodeAST *node, const char *opcode) {
  free(node->opC);
  node->opC = dup_string(opcode);
}

void NodeAST_add_reg_operand(NodeAST *node, const char *operand,
                             size_t regIndex) {
  (void)operand;
  node_add_operand_kind(node, OPERAND_REG);

  *(node->nreg) += 1;
  size_t **new_regs = realloc(node->regs, *(node->nreg) * sizeof(size_t *));
  if (new_regs == NULL)
    errx(1, "Can't reallocate memory for the registers array!");
  node->regs = new_regs;

  node->regs[*(node->nreg) - 1] = malloc(sizeof(size_t));
  if (node->regs[*(node->nreg) - 1] == NULL)
    errx(1, "Can't allocate memory for the register index!");

  *(node->regs[*(node->nreg) - 1]) = regIndex;
}

void NodeAST_add_mem_operand(NodeAST *node, const char *operand,
                             const char *specifier) {
  size_t len = strlen(operand);
  if (len >= 2 && operand[0] == '[' && operand[len - 1] == ']') {
    char *copy = malloc(len - 1);
    if (copy == NULL)
      errx(1, "Can't allocate memory for the memory operand!");
    memcpy(copy, operand + 1, len - 2);
    copy[len - 2] = '\0';
    free(node->mOp);
    node->mOp = copy;
  } else {
    free(node->mOp);
    node->mOp = dup_string(operand);
  }
  node_add_operand_kind(node, OPERAND_MEM);

  if (specifier != NULL) {
    free(node->spe);
    node->spe = dup_string(specifier);
  }
}

void NodeAST_add_imm_operand(NodeAST *node, const char *operand,
                             uint16_t value) {
  (void)operand;
  node_add_operand_kind(node, OPERAND_IMM);

  free(node->imm);
  node->imm = malloc(sizeof(uint16_t));
  if (node->imm == NULL)
    errx(1, "Can't allocate memory for the immediate data!");

  *(node->imm) = value;
}

void NodeAST_finalize(NodeAST *node, const char *asmLine, size_t opLen) {
  free(node->ASM);
  node->ASM = dup_string(asmLine);
  node->opLen = opLen;
}

void push16_stack(uint16_t val) {
  data_mem[--(*sp)] = val >> 8;
  data_mem[--(*sp)] = val;
}

uint16_t pop16_stack() {
  uint16_t val = data_mem[(*sp)++];
  val |= data_mem[(*sp)++] << 8;
  return val;
}