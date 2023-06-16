#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

#define BUFFER_SIZE 131072    // 128 KiB
#define HEADER_SIZE 32        // Bytes 0-31 are the header
#define HEADER_PROGRAM_SIZE 8 // Program size starting at byte 8 (4B)
#define HEADER_DATA_SIZE HEADER_PROGRAM_SIZE + 4 // Data size follows PS (4B)
#define COMMAND_SIZE 256                         // 256 bytes

/// @brief Dumps the given file and returns (by ref) the buffer, buffer size,
/// and program size
/// @param path The path to the file to dump
/// @param buffer The buffer to dump the file into (modified by ref)
/// @param buffer_size The size of the buffer (modified by ref)
void hexdump(const char *path, unsigned char *buffer, size_t *buffer_size,
             size_t *text_size, size_t *data_size);

char **get_cout(char *command, size_t *lineCount);