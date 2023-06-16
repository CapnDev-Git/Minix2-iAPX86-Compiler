#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dump.h"
#include "globals.h"
#include "lexer.h"
#include "parser.h"
#include "prints.h"

// Message structure
typedef struct {
  uint16_t t;
  size_t nbytes;
  size_t data;
} Message;

// Sizes
#define INSTR_SIZE 47
#define FLAG_SIZE 4
#define SYSCALL_SIZE 64
#define DATA_LINE_SIZE 16

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

/// @brief Interpret the given node
/// @param node The node to interpret
/// @param program The program to be interpreted
/// @param data The data to be used
int interpret(NodeAST *node, unsigned char *program, unsigned char *data);
