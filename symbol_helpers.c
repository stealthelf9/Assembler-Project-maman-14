#include "symbol_helpers.h"

int isSymbolValid(char *symbol_name, SymbolNode *head, int line_number, int is_extern) {
    int index = 0;
    int num_reserved_words = ARRAY_SIZE(reserved_words);

    SymbolNode *node = head;
    SymbolNode *existing_symbol;
            if (!isalpha(symbol_name[0])) {
            fprintf(stderr, "ERROR at line %d: Symbol name must start with an English letter.\n", line_number);
            return 1;
        }
    while(index < strlen(symbol_name)) {
        if (!isalnum(symbol_name[index])) {
            fprintf(stderr, "ERROR at line %d: Symbol name must only have English letters or numbers.\n", line_number);
            return 1;
        }
        index++;
    }
    index = 0;

    while(index < num_reserved_words) {
        if (strcmp(symbol_name, reserved_words[index]) == 0) {
            fprintf(stderr, "ERROR at line %d: Symbol shares the name with a reserved word.\n", line_number);
            return 1;
        }
        index++;
    }
    
/* If the symbol already exists, we need to see if it's of type .extern or a label. */
    existing_symbol = findSymbol(symbol_name, head);
    if (existing_symbol != NULL) {
        
        if (is_extern == 1) {
            /* If we are currently trying to add .extern K */
            if (existing_symbol->is_extern == 0) {
                fprintf(stderr, "ERROR at line %d: Symbol '%s' cannot be declared .extern because it is already a local label.\n", line_number, symbol_name);
                return 1;
            }
            /* If it is already an extern, we do nothing. Multiple .externs are legal. */
        } 
        else {
            /* If we are currently trying to add a local label 'K:' */
            if (existing_symbol->is_extern == 1) {
                fprintf(stderr, "ERROR at line %d: Symbol '%s' cannot be defined as a local label because it is already declared as .extern.\n", line_number, symbol_name);
                return 1;
            }
            else {
                fprintf(stderr, "ERROR at line %d: Duplicate local label '%s' found.\n", line_number, symbol_name);
                return 1;
            }
        }
    }
    
    return 0; /* Symbol is valid. */

}

SymbolNode *findSymbol(char *symbol_name, SymbolNode *head) {
    SymbolNode *node = head;

    while(node != NULL) {
        if (strcmp(node->name, symbol_name) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void freeSymbolList(SymbolNode *head) {
    int index;
    SymbolNode *node = head;
    SymbolNode *next_node;

    while (node != NULL) {
        next_node = node->next;
        if (node->name != NULL)
            free(node->name);

        free(node);
        node = next_node;
    }
}

SymbolNode *setSymbol(char *symbol_name, SymbolNode **head, SymbolNode **tail, int line_number) {
    SymbolNode *symbol = NULL;
    symbol = calloc(1, sizeof(SymbolNode));

    if (symbol == NULL) {
        fprintf(stderr, "ERROR at line %d: Out of memory.\n", line_number);
        return NULL;
    }

    symbol->name = calloc(1, strlen(symbol_name) + 1);
    if (symbol->name == NULL) {
        fprintf(stderr, "ERROR at line %d: Out of memory.\n", line_number);
        free(symbol);
        return NULL;
    }

    strcpy(symbol->name, symbol_name);
    symbol->next = NULL;

    if (*head == NULL) {
        *head = symbol;
        *tail = symbol;
    }
    else {
        (*tail)->next = symbol;
        *tail = symbol;
    }
    return symbol;
}

int processSymbol(SymbolNode **head, SymbolNode **tail, SymbolNode **symbol, char *symbol_name, int line_number, int *error_flag, int is_extern) {
    int symbol_validity;

    symbol_validity = isSymbolValid(symbol_name, *head, line_number, is_extern);
    if (symbol_validity == 1) {
        *error_flag = 1;
        return 1;
    }

    *symbol = setSymbol(symbol_name, head, tail, line_number);
    
    if (*symbol == NULL) {
        return -1;
    }
    return 0;
}