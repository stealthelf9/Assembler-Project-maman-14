#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "utils.h"
#include "symbol_helpers.h"

int firstPass(FILE *file, char *file_name, char *are_image, unsigned int *code_image, unsigned int *data_image, int *ic, int *dc, SymbolNode **head);

#endif