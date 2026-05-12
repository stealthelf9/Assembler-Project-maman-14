#ifndef MACRO_HELPERS_H
#define MACRO_HELPERS_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "structs.h"
#include "symbol_helpers.h"

/**
 * isMacroValid
 * Checks if the macro name starts with an english letter, has only numbers, underscores or letters inside the name
 * and doesn't share the same name of an existing macro, symbol or reserved word.
 * @param macro_name The name of the macro that's being checked.
 * @param mac_head The head of the macro list.
 * @param sym_head The head of the symbol list.
 * @param line_number The line number of the code that the macro name was found.
 * @return Returns 1 if it's an invalid name, 0 if it is.
 */
int isMacroValid(char *macro_name, MacroNode *mac_head, SymbolNode *sym_head, int line_number);

/**
 * findMacro
 * Finds the macro node from its name.
 * @param macro_name The macro name.
 * @param head The head of the macro list.
 * @return NULL if the macro wasn't found or it returns the pointer to the macro node if it was found.
 */
MacroNode *findMacro(char *macro_name, MacroNode *head);

/**
 * expandMacro
 * Expands the macro into code and outputs it into the .am file
 * @param output The output .am file.
 * @param macro The macro node that has the code.
 */
void expandMacro(FILE *output, MacroNode *macro);

/**
 * freeMacroList
 * Frees the macro list from the memory.
 * @param head The head of the macro list.
 */
void freeMacroList(MacroNode *head);

#endif