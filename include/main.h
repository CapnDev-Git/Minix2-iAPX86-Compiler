#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dump.h"
#include "globals.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "prints.h"

#define MAX_INSTR 100000

/// @brief Main function
/// @param argc Number of arguments
/// @param argv Arguments
/// @return Exit code (0 for success, 1 for failure)
int main(int argc, char **argv);