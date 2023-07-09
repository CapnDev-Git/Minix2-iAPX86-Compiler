#pragma once

#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/globals.h"
#include "../include/prints.h"

#define MAX_LENGTH 1000000

char *get_cout(const char *command);
void cmp_files(const char *filename, int verbose, int interpret);
void cmp_files_dir(const char *directory, int verbose, int interpret);
int main(int argc, char **argv);