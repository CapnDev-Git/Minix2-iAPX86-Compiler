#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "prints.h"

Vector get_args(int argc, char **argv);
Vector get_envp();
void process_args(Vector *args, Vector *envp);
void setup_memory(int argc, char **argv);