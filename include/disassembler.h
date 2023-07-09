#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "globals.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "patterns.h"
#include "prints.h"

/// @brief Gets the effective address of the instruction
/// @param p The program buffer
/// @param a The address of the instruction
/// @param mod The mod field (2 bits)
/// @param rm The rm field (3 bits)
/// @param ea The effective address (modified by ref)
/// @param ds The data size (modified by ref)
void get_adm(size_t a, unsigned char mod, unsigned char rm, char *ea,
             size_t *ds);

/// @brief Translates the given program buffer
void disassemble();