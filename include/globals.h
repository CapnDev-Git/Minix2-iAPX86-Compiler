#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// AST node structure
typedef struct {
  size_t adr;
  char *ASM; // to remove
  char *opC;
  size_t opLen;

  char *op1;
  char *op2;

  char *spe;
  char *mOp;
  uint16_t *imm;

  size_t *nreg;
  size_t **regs;
} NodeAST;

// System call structure
typedef struct {
  uint16_t type;
  const char *name;
} SyscallType;

// System call return structure
typedef struct {
  uint16_t type;
  size_t returnValue;
} SyscallReturn;

// Message structure
typedef struct {
  uint16_t t;
  size_t nbytes;
  size_t data;
} Message;

// Vector structure
typedef struct {
  void **data;
  uint16_t size;
  uint16_t capacity;
} Vector;

// Global constants
#define BUFFER_SIZE 131072    // 128 KiB
#define HEADER_SIZE 32        // Bytes 0-31 are the header
#define HEADER_PROGRAM_SIZE 8 // Program size starting at byte 8 (4B)
#define HEADER_DATA_SIZE HEADER_PROGRAM_SIZE + 4 // Data size follows PS (4B)
#define COMMAND_SIZE 256                         // 256 bytes
#define MEMORY_SIZE 0x10000
#define MAX_INSTR 100000
#define MAX_ARGS 50

// Disassembler sizes
#define EA_STRING_SIZE 20 // Chosen arbitrarily
#define R8_SIZE 8
#define R16_SIZE 8
#define SR_SIZE 4
#define OPCODES_SIZE 16
#define DESTS_SIZE 8

// Interpreter sizes
#define INSTR_SIZE 47
#define FLAG_SIZE 4
#define SYSCALLS_SIZE 4 // so far, might change
#define DATA_LINE_SIZE 16
#define REG_SIZE 8
#define ALL_REG_SIZE 16

// Interpreter exit codes
#define EXIT_CONTINUE 0
#define EXIT_SYSCALL -1
#define EXIT_IPCHANGED -2

// Global variables
extern int INTERPRET;

extern size_t TEXT_SIZE;
extern size_t DATA_SIZE;
extern size_t TEXT_BEG;
extern size_t DATA_BEG;
extern unsigned char text_mem[MEMORY_SIZE];
extern unsigned char data_mem[MEMORY_SIZE];

extern char ASM[MEMORY_SIZE][256];
extern size_t ASM_MAX_INDEX;

extern char INTPTR[MEMORY_SIZE][256];
extern size_t INTPTR_MAX_INDEX;

extern const char *REG8[R8_SIZE];
extern const char *REG16[R16_SIZE];
extern const char *SEGREG[SR_SIZE];

extern const char *instructions[INSTR_SIZE];
extern const char *registers[ALL_REG_SIZE];

extern const SyscallType syscallTypes[SYSCALLS_SIZE];
extern const SyscallReturn syscallReturns[SYSCALLS_SIZE];

extern uint16_t regs[REG_SIZE];
extern unsigned char flags[FLAG_SIZE];
extern uint16_t *sp;
extern uint16_t IP;

// Operations
#define OP_PLUS '+'
#define OP_MINUS '-'
#define OP_MULT '*'
#define OP_DIV '/'
#define OP_CMP 'c'
#define OP_AND '&'

// Instructions
#define ADC 0
#define ADD 1
#define AND 2
#define CALL 3
#define CBW 4
#define CLD 5
#define CMP 6
#define CWD 7
#define DEC 8
#define DIV 9
#define HLT 10
#define IN 11
#define INC 12
#define INT 13
#define JAE 14
#define JB 15
#define JBE 16
#define JE 17
#define JLE 18
#define JNB 19
#define JNE 20
#define JNL 21
#define JNLE 22
#define JNBE 23
#define JMP 24
#define JL 25
#define LEA 26
#define LOOP 27
#define MOV 28
#define MUL 29
#define NEG 30
#define OR 31
#define POP 32
#define PUSH 33
#define RCL 34
#define REP 35
#define RET 36
#define SAR 37
#define SBB 38
#define SHL 39
#define SHR 40
#define STD 41
#define SUB 42
#define TEST 43
#define XCHG 44
#define XOR 45
#define UNDEFINED 46

// Registers
#define AX 0
#define BX 1
#define CX 2
#define DX 3
#define SP 4
#define BP 5
#define SI 6
#define DI 7

// Flags
#define OF 0
#define SF 1
#define ZF 2
#define CF 3

// System calls
#define EXIT "exit"
#define WRITE "write"
#define BRK "brk"
#define IOCTL "ioctl"

// Prints
#define HEX_PER_LINE 16              // 16 bytes per line when printing
#define BYTE_PER_LINE 4              // 8 bytes per line when printing
#define HEX_FORMAT_U "%02X "         // Format for printing hex
#define HEX_FORMAT_L "%02x "         // Format for printing hex
#define BIN_FORMAT "%08X "           // Format for printing binary
#define ADDRESS_FORMAT "%08zx  "     // Format for printing address
#define DIS_ADDRESS_FORMAT "%04lx: " // Format for printing disassembler address
#define INSTRUCTION_FORMAT "%02x"    // Format for printing hex
#define NO_CHAR '-'                  // Character to print if not printable
#define ASCII_SEPARATOR "|"

#define ADR_SEPERATOR "--------" // Separator between address and hex
#define ADR_HEADER "address "    // Header for address column

#define SPACE " "   // Regular space (for dump printing)
#define SPACE2 "  " // Double space (for dump printing)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  ((byte)&0x80 ? '1' : '0'), ((byte)&0x40 ? '1' : '0'),                        \
      ((byte)&0x20 ? '1' : '0'), ((byte)&0x10 ? '1' : '0'),                    \
      ((byte)&0x08 ? '1' : '0'), ((byte)&0x04 ? '1' : '0'),                    \
      ((byte)&0x02 ? '1' : '0'), ((byte)&0x01 ? '1' : '0')

#define REGS_STATUS "%04x %04x %04x %04x %04x %04x %04x %04x "
#define FLAGS_STATUS "%c%c%c%c "
#define LINE_STATUS "%s"
#define MEMORY_CONTENT "%s\n"

// Helpers functions
void free_2d(char **array, size_t len);
size_t get_index(const char **array, size_t size, const char *element);

// Vectors functions
void vector_init(Vector *vector);
void vector_pushback(Vector *vector, void *element);
void vector_free(Vector *vector);

// NodeAST functions
void NodeAST_init(NodeAST *node);

// Stack functions
void push16_stack(uint16_t val);