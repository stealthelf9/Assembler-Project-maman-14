#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "macro_helpers.h"

/**
 * preAssembler
 * This is the pre-assembler of the assembler. It expands macros and ommits the declarations from the output.
 * @param file The .as file.
 * @param file_name the filename of the .as file.
 * @return 1 if there's an error, 0 if successful.
 */
int preAssembler(FILE *file, char *file_name);

#endif