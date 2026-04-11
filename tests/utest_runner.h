#pragma once

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/prints.h"

#define MAX_LENGTH 1000000

char *get_cout(const char *command);
void run_compare_file(const char *filename, int verbose, int interpret);
void run_compare_directory(const char *directory, int verbose, int interpret);