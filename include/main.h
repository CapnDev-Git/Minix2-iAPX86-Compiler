#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disassembler.h"
#include "dump.h"
#include "globals.h"
#include "interpreter.h"
#include "lexer.h"
#include "memory.h"
#include "parser.h"
#include "patterns.h"
#include "print_patterns.h"
#include "prints.h"

/// @brief Main function
/// @param argc Number of arguments
/// @param argv Arguments
/// @return Exit code (0 for success, 1 for failure)
int main(int argc, char **argv);