#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern size_t TEXT_SIZE;
extern size_t DATA_SIZE;
extern size_t TEXT_BEG;
extern size_t TEXT_END;
extern size_t DATA_BEG;
extern size_t DATA_END;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void free_2d(char **array, size_t len);
size_t get_index(const char **array, size_t size, const char *element);