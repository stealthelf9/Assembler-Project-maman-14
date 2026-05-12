#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "utils.h"
#include "symbol_helpers.h"

/**
 * firstPass
 * This is the first pass. It uses the file processed by the pre-assembler, it saves the commands into the code image, strings and numbers into the data image
 * and also saves the binary values of the commands that only have immediate or register modes inside the commands into the code image and ARE image.
 * @param file The processed file from the pre-assembler.
 * @param file_name The file name.
 * @param are_image The ARE image.
 * @param code_image The code image.
 * @param data_image The data image.
 * @param ic The instruction counter.
 * @param dc The data counter.
 * @param head The head of the symbol table.
 * @return 1 for faliure, 0 for success. 
 */
int firstPass(FILE *file, char *file_name, char *are_image, unsigned int *code_image, unsigned int *data_image, int *ic, int *dc, SymbolNode **head);

#endif