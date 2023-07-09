#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disassembler.h"
#include "globals.h"
#include "print_patterns.h"
#include "prints.h"

/// @brief Parsing function for the 0x0 pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x0(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0x1 pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x1(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0x2 pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x2(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0x3 pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x3(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0x4 pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x4(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0x5 pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x5(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0x6 pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x6(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0x7 pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x7(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0x8 pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x8(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0x9 pattern (current opcode 4-MSBs)

/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0x9(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0xa pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0xa(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0xb pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0xb(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0xc pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0xc(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0xd pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0xd(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0xe pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0xe(size_t a, unsigned char LSB4);

/// @brief Parsing function for the 0xf pattern (current opcode 4-MSBs)
/// @param a The current address
/// @param LSB4 The current opcode 4-LSBs

void f0xf(size_t a, unsigned char LSB4);