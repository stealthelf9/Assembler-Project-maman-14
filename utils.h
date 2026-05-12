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

/**
 * findCommand
 * Finds the command int the command table based on the command name.
 * @param command_name The command name you want to find.
 * @return -1 if the command wasn't found or the index of the command table if the command was found.
 */
int findCommand(char *command_name);

/**
 * getAddressMode
 * Gets the address mode of the operand.
 * @param operand The operand.
 * @return The address mode of the operand.
 */
int getAddressMode(char *operand);
#endif
