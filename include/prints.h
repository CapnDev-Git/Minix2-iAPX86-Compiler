#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

#define HEX_PER_LINE 16              // 16 bytes per line when printing
#define BYTE_PER_LINE 4              // 8 bytes per line when printing
#define HEX_FORMAT_U "%02X "         // Format for printing hex
#define HEX_FORMAT_L "%02x "         // Format for printing hex
#define BIN_FORMAT "%08X "           // Format for printing binary
#define ADDRESS_FORMAT "%08zx  "     // Format for printing address
#define DIS_ADDRESS_FORMAT "%04lx: " // Format for printing disassembler address
#define INSTRUCTION_FORMAT "%02x"    // Format for printing hex
#define NO_CHAR '.'                  // Character to print if not printable
#define ASCII_SEPARATOR "|"

#define ADR_SEPERATOR "--------" // Separator between address and hex
#define ADR_HEADER "address "    // Header for address column

#define IS1 "            " // Instruction spacing for 1 byte
#define IS2 "          "   // Instruction spacing for 2 bytes
#define IS3 "        "     // Instruction spacing for 3 bytes
#define IS4 "      "       // Instruction spacing for 4 bytes
#define IS5 "    "         // Instruction spacing for 5 bytes
#define IS6 "  "           // Instruction spacing for 6 bytes
#define SPACE " "          // Regular space (for dump printing)
#define SPACE2 "  "        // Double space (for dump printing)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  ((byte)&0x80 ? '1' : '0'), ((byte)&0x40 ? '1' : '0'),                        \
      ((byte)&0x20 ? '1' : '0'), ((byte)&0x10 ? '1' : '0'),                    \
      ((byte)&0x08 ? '1' : '0'), ((byte)&0x04 ? '1' : '0'),                    \
      ((byte)&0x02 ? '1' : '0'), ((byte)&0x01 ? '1' : '0')

#define REGS_STATUS "%04lx %04lx %04lx %04lx %04lx %04lx %04lx %04lx "
#define FLAGS_STATUS "%c%c%c%c "
#define LINE_STATUS "%s\n"

/// @brief Print the hexadecimals and ASCII representation of a buffer
/// @param buffer The buffer to print
/// @param buffer_size The size of the buffer
void print_hexdump(const unsigned char *buffer, size_t buffer_size);

/// @brief Print the registers header
void print_regs_header();

/// @brief Print the registers status
/// @param r The registers
/// @param f The flags
/// @param l The line
void print_regs_status(size_t *r, char *f, char *l);