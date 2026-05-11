#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "structs.h"

#define CHECK_MEM(ic, dc) \
    if (((dc) + (ic)) >= MAX_MEMORY_SIZE) { \
        fprintf(stderr, "FATAL ERROR: memory overflow\n"); \
        return -1; \
    }

int findCommand(char *command_name);
int getAddressMode(char *operand);
#endif
