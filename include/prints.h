#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

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
/// @param m The memory content (optional)
void print_regs_status(uint16_t *r, char *f, char *l, char *m);

/// @brief Print OK
/// @param filename The filename
void printOK(const char *filename);

/// @brief Print KO
/// @param filename The filename
void printKO(const char *filename);

/// @brief Print the diff between the error line and the reference
/// @param errorLine The error line
/// @param ref The reference
void printDiff(const char *errorLine, const char *ref);

void print4b(const unsigned char *p, size_t a, size_t il, size_t *ip,
             char *line);