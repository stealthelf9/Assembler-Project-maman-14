#ifndef SYMBOL_HELPERS_H
#define SYMBOL_HELPERS_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "structs.h"

int isSymbolValid(char *symbol_name, SymbolNode *head, int line_number, int is_extern);
int findCommand(char *command_name);
SymbolNode *findSymbol(char *symbol_name, SymbolNode *head);
SymbolNode *setSymbol(char *symbol_name, SymbolNode **head, SymbolNode **tail , int line_number);
void freeSymbolList(SymbolNode *head);
#endif