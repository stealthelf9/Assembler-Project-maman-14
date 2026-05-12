#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "utils.h"
#include "symbol_helpers.h"

/**
 * secondPass 
 * This is the second pass of the assembler. It fills in the the images that are from other modes that are from the symbol table created by the first pass.
 * @param file The processed file from the pre-assembler.
 * @param file_name The file name.
 * @param are_image The image for all the A R E letters.
 * @param code_image The code image.
 * @param data_image The data image.
 * @param dc The data counter.
 * @param head The head to the linked list of the symbols.
 * @return 1 for faliure, 0 for success. 
 */
int secondPass(FILE *file, char *file_name, char *are_image, unsigned int *code_image, unsigned int *data_image, int dc, SymbolNode *head);

#endif