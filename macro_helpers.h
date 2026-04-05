#ifndef MACRO_HELPERS_H
#define MACRO_HELPERS_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "structs.h"
#include "symbol_helpers.h"

int isMacroValid(char *macro_name, MacroNode *mac_head, SymbolNode *sym_head, int line_number);
MacroNode *findMacro(char *macro_name, MacroNode *head);
void expandMacro(FILE *output, MacroNode *macro);
void freeMacroList(MacroNode *head);

#endif