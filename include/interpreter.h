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
#include "prints.h"

/// @brief Interpret the given node
/// @param node The node to interpret
int interpret(NodeAST *node);
