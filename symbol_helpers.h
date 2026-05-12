#ifndef SYMBOL_HELPERS_H
#define SYMBOL_HELPERS_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "structs.h"

/**
 * isSymbolValid
 * Checks if the symbol name starts with an english letter, has only numbers or letters inside the name
 * and doesn't share the same name of an existing symbol or reserved word.
 * @param symbol_name The name of the macro that's being checked.
 * @param head The head of the symbol list.
 * @param line_number The line number of the code that the symbol name was found.
 * @param is_extern If the symbol is an external symbol
 * @return 1 if it's an invalid name, 0 if it is.
 */
int isSymbolValid(char *symbol_name, SymbolNode *head, int line_number, int is_extern);

/**
 * processSymbol
 * Processes the symbol name, checks if its name is valid, and then sets it to a symbol pointer.
 * @param head The head of the symbol list.
 * @param tail The tail of the symbol list.
 * @param symbol The symbol node that will be set as a pointer to the created symbol.
 * @param line_number The line number of where the symbol is declared.
 * @param error_flag The error flag.
 * @param is_extern If the symbol is an external symbol.
 * @return 1 If the symbol has an invalid name, -1 if the ram is full, 0 if processing was a success.
 */
int processSymbol(SymbolNode **head, SymbolNode **tail, SymbolNode **symbol, char *symbol_name, int line_number, int *error_flag, int is_extern);

/**
 * findSymbol
 * Finds the symbol node from its name.
 * @param symbol_name The sybmol name.
 * @param head The head of the symbol list.
 * @return NULL if the symbol wasn't found or it returns the pointer to the symbol node if it was found.
 */
SymbolNode *findSymbol(char *symbol_name, SymbolNode *head);

/**
 * setSymbol
 * Adds a new symbol to the symbol list.
 * @param symbol_name The name of the symbol that wants to be added.
 * @param head The head of the symbol list.
 * @param tail The tail of the symbol list.
 * @param line_number The line number of where the symbol is being added.
 * @return NULL if the memory is full and the symbol cannot be created or the pointer to the symbol that was created successfully
 */
SymbolNode *setSymbol(char *symbol_name, SymbolNode **head, SymbolNode **tail , int line_number);

/**
 * freeSymbolList
 * Frees the symbol list from the memory.
 * @param head The head of the symbol list.
 */
void freeSymbolList(SymbolNode *head);
#endif