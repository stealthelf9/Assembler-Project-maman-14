#ifndef SYMBOL_HELPERS_H
#define SYMBOL_HELPERS_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "structs.h"

int isSymbolValid(char *symbol_name, SymbolNode *head, int line_number, int is_extern);
int findCommand(char *command_name);
int processSymbol(SymbolNode **head, SymbolNode **tail, SymbolNode **symbol, char *symbol_name, int line_number, int *error_flag, int is_extern);
SymbolNode *findSymbol(char *symbol_name, SymbolNode *head);
SymbolNode *setSymbol(char *symbol_name, SymbolNode **head, SymbolNode **tail , int line_number);
void freeSymbolList(SymbolNode *head);
#endif