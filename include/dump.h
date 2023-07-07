#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

/// @brief Dumps the given file and returns (by ref) the buffer, buffer size,
/// and program size
/// @param path The path to the file to dump
void hexdump(const char *path);