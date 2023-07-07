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

const char *registers[REG_SIZE] = {"ax", "bx", "cx", "dx",
                                   "sp", "bp", "si", "di"};

const SyscallType syscallTypes[SYSCALLS_SIZE] = {
    {1, "exit"}, {4, "write"}, {17, "brk"}, {54, "ioctl"}};

const SyscallReturn syscallReturns[SYSCALLS_SIZE] = {
    {1, 0}, {4, 6}, {17, 0}, {54, -1}};

unsigned char text_mem[MEMORY_SIZE] = {0}; // reset the text memory
unsigned char data_mem[MEMORY_SIZE] = {0}; // reset the data memory
uint16_t regs[REG_SIZE] = {0};
unsigned char flags[FLAG_SIZE] = {'-', '-', '-', '-'};
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
  node->adr = 0;
  node->ASM = NULL;
  node->opC = NULL;
  node->opLen = 0;

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

  node->regs = malloc(sizeof(size_t *));
  if (node->regs == NULL)
    errx(1, "Can't allocate memory for the registers array!");
  *(node->regs) = NULL;
}