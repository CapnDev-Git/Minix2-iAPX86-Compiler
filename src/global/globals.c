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
  node->sizeSpec = OPERAND_SIZE_NONE;
  node->memKind = MEMORY_KIND_NONE;
  node->memRegCount = 0;
  node->memRegs[0] = 0;
  node->memRegs[1] = 0;
  node->memDisp = 0;
  node->memDirect = 0;
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
  free(node->imm);

  if (node->regs != NULL) {
    for (size_t j = 0; j < *(node->nreg); j++)
      free(node->regs[j]);
    free(node->regs);
  }

  free(node->nreg);
  node->ASM = NULL;
  node->opC = NULL;
  node->sizeSpec = OPERAND_SIZE_NONE;
  node->memKind = MEMORY_KIND_NONE;
  node->memRegCount = 0;
  node->memRegs[0] = 0;
  node->memRegs[1] = 0;
  node->memDisp = 0;
  node->memDirect = 0;
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

void NodeAST_set_size_spec(NodeAST *node, OperandSize sizeSpec) {
  node->sizeSpec = sizeSpec;
}

static OperandSize parse_operand_size_spec(const char *specifier) {
  if (specifier == NULL)
    return OPERAND_SIZE_NONE;
  if (!strcmp(specifier, "byte"))
    return OPERAND_SIZE_BYTE;
  if (!strcmp(specifier, "short"))
    return OPERAND_SIZE_SHORT;
  return OPERAND_SIZE_NONE;
}

static int is_register_token(const char *token) {
  return get_index(registers, ALL_REG_SIZE, token) != (size_t)-1;
}

static void parse_memory_operand(NodeAST *node, const char *operand) {
  char *copy = dup_string(operand);
  size_t len = strlen(copy);

  if (len >= 2 && copy[0] == '[' && copy[len - 1] == ']') {
    copy[len - 1] = '\0';
    memmove(copy, copy + 1, len - 1);
  }

  node->memKind = MEMORY_KIND_NONE;
  node->memRegCount = 0;
  node->memRegs[0] = 0;
  node->memRegs[1] = 0;
  node->memDisp = 0;
  node->memDirect = 0;

  char *cursor = copy;
  int sign = 1;
  while (*cursor != '\0') {
    char *term = cursor;
    while (*cursor != '\0' && *cursor != '+' && *cursor != '-')
      cursor++;

    char saved = *cursor;
    *cursor = '\0';

    if (*term != '\0') {
      if (is_register_token(term)) {
        if (node->memRegCount < 2)
          node->memRegs[node->memRegCount++] =
              get_index(registers, ALL_REG_SIZE, term);
      } else {
        long value = strtol(term, NULL, 16);
        if (node->memRegCount == 0 && node->memKind == MEMORY_KIND_NONE) {
          node->memKind = MEMORY_KIND_DIRECT;
          node->memDirect = (uint16_t)value;
        } else {
          node->memDisp += (int16_t)(sign * value);
          node->memKind = MEMORY_KIND_REGISTER;
        }
      }
    }

    if (saved == '\0')
      break;
    sign = (saved == '+') ? 1 : -1;
    cursor++;
  }

  if (node->memRegCount > 0)
    node->memKind = MEMORY_KIND_REGISTER;

  free(copy);
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
  node_add_operand_kind(node, OPERAND_MEM);
  node->sizeSpec = parse_operand_size_spec(specifier);
  parse_memory_operand(node, operand);
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