#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disassembler.h"
#include "globals.h"
#include "patterns.h"
#include "prints.h"

/// @brief General purpose printing function for instructions using the sw flags
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param ea The effective address
/// @param LSB4 The least significant 4 bits of the instruction
/// @param ds The data size
void printsw(size_t a, char *instr, size_t *ip, char *ea, unsigned char LSB4,
             size_t *ds);

/// @brief General purpose printing function for instructions using the vw flags
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param LSB4 The least significant 4 bits of the instruction
/// @param ds The data size
void printvw(size_t a, char *instr, size_t *ip, unsigned char LSB4, size_t *ds);

/// @brief General purpose printing function for instructions using the dw flags
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param LSB4 The least significant 4 bits of the instruction
/// @param ds The data size
void printdw(size_t a, char *instr, size_t *ip, unsigned char LSB4, size_t *ds);

/// @brief General purpose printing function for instructions moving immediate
/// data to accumulators
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param w The w flag
/// @param ip The instruction pointer
void print_immtoacc(size_t a, char *instr, unsigned char w, size_t *ip);

/// @brief General purpose printing function for instructions moving immediate
/// to reg/memory
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param ea The effective address
/// @param LSB4 The least significant 4 bits of the instruction
/// @param ds The data size
void print_immtorm(size_t a, char *instr, size_t *ip, char *ea,
                   unsigned char LSB4, size_t *ds);

/// @brief General purpose printing function for instructions get reg/memory
/// from immediate data
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param ea The effective address
/// @param LSB4 The least significant 4 bits of the instruction
/// @param ds The data size
void print_rmtoimm(size_t a, char *instr, size_t *ip, char *ea,
                   unsigned char LSB4, size_t *ds);

/// @brief General purpose printing function for instructions using 16bits
/// reg/memory
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param rm The rm field
/// @param ip The instruction pointer
void print_rm16(size_t a, char *instr, unsigned char rm, size_t *ip);

/// @brief General purpose printing function for conditional jumps instructions
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
void print_condjmps(size_t a, char *instr, size_t *ip);

/// @brief General purpose printing function for instructions using rep prefix
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param w The w flag
void print_repmanip(size_t a, char *instr, size_t *ip, unsigned char w);

/// @brief General purpose printing function for instructions only without any
/// parameters
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param il The instruction length
void print_single(size_t a, char *instr, size_t *ip, size_t il);

/// @brief General purpose printing function for instructions having fixed port
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param w The w flag
void print_portfixed(size_t a, char *instr, size_t *ip, unsigned char w);

/// @brief General purpose printing function for instructions having variable
/// port
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param w The w flag
void print_portvar(size_t a, char *instr, size_t *ip, unsigned char w);

/// @brief General purpose printing function for loops instructions
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
void print_loop(size_t a, char *instr, size_t *ip);

/// @brief General purpose printing function for call/jmp instructions
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
void print_calljmp(size_t a, char *instr, size_t *ip);

/// @brief General purpose printing function for string manipulation
/// instructions
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param w The w flag
/// @param ip The instruction pointer
void print_stringmanip(size_t a, char *instr, unsigned char w, size_t *ip);

/// @brief General purpose printing function for instructions using the w flag
/// and starting with 0xf
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param ea The effective address
/// @param LSB4 The least significant 4 bits of the instruction
/// @param ds The data size
void print_fw(size_t a, char *instr, size_t *ip, char *ea, unsigned char LSB4,
              size_t *ds);

/// @brief General purpose printing function for instructions depending on the
/// rm field
/// @param a The address of the instruction
/// @param instr The instruction name
/// @param ip The instruction pointer
/// @param ea The effective address
/// @param ds The data size
void print_frm(size_t a, char *instr, size_t *ip, char *ea, size_t *ds);