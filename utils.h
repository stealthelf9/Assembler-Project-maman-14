#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "structs.h"

#define MAX_FILE_SIZE 256 /* Modern file system limit. */
#define CHECK_MEM(ic, dc) \
    if (((dc) + (ic)) >= 4096) { \
        fprintf(stderr, "FATAL ERROR: memory overflow\n"); \
        return -1; \
    }

int findCommand(char *command_name);
#endif
