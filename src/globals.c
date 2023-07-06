#include "globals.h"

// Initialize the global variables
size_t TEXT_SIZE = 0;
size_t DATA_SIZE = 0;
size_t TEXT_BEG = 0;
size_t DATA_BEG = 0;

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