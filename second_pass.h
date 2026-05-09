#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "utils.h"
#include "symbol_helpers.h"

int secondPass(FILE *file, char *file_name, char *are_image, unsigned int *code_image, unsigned int *data_image, int dc, SymbolNode *head);

#endif