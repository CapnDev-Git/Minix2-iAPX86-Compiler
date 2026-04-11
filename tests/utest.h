#pragma once

#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/globals.h"
#include "../include/prints.h"

#define MAX_LENGTH 1000000

void run_compare_file(const char *filename, int verbose, int interpret);
void run_compare_directory(const char *directory, int verbose, int interpret);
int main(int argc, char **argv);