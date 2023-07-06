#pragma once

#include <ctype.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "prints.h"

Vector get_envp();
void process_args(Vector *args, Vector *envp);