#include "symbol_helpers.h"

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
int isSymbolValid(char *symbol_name, SymbolNode *head, int line_number, int is_extern) {
    int index = 0;
    int num_reserved_words = ARRAY_SIZE(reserved_words);

    SymbolNode *existing_symbol;

    /* Checks if the symbol starts with an english letter and has only numbers or letters inside the name. */
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
int processSymbol(SymbolNode **head, SymbolNode **tail, SymbolNode **symbol, char *symbol_name, int line_number, int *error_flag, int is_extern) {
    int symbol_validity;

    /* Check if the symbol is valid */
    symbol_validity = isSymbolValid(symbol_name, *head, line_number, is_extern);
    if (symbol_validity == 1) {
        *error_flag = 1;
        return 1;
    }

    /* Set the symbol if it has a valid name */
    *symbol = setSymbol(symbol_name, head, tail, line_number);
    if (*symbol == NULL) {
        return -1; /* Out of memory, fatal error */
    }
    return 0;
}

/**
 * findSymbol
 * Finds the symbol node from its name.
 * @param symbol_name The sybmol name.
 * @param head The head of the symbol list.
 * @return NULL if the symbol wasn't found or it returns the pointer to the symbol node if it was found.
 */
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

/**
 * setSymbol
 * Adds a new symbol to the symbol list.
 * @param symbol_name The name of the symbol that wants to be added.
 * @param head The head of the symbol list.
 * @param tail The tail of the symbol list.
 * @param line_number The line number of where the symbol is being added.
 * @return NULL if the memory is full and the symbol cannot be created or the pointer to the symbol that was created successfully
 */
SymbolNode *setSymbol(char *symbol_name, SymbolNode **head, SymbolNode **tail, int line_number) {
    SymbolNode *symbol = NULL;

    /* Create the symbol */
    symbol = calloc(1, sizeof(SymbolNode));
    if (symbol == NULL) {
        fprintf(stderr, "ERROR at line %d: Out of memory.\n", line_number);
        return NULL;
    }

    /* Add the symbol name */
    symbol->name = calloc(1, strlen(symbol_name) + 1);
    if (symbol->name == NULL) {
        fprintf(stderr, "ERROR at line %d: Out of memory.\n", line_number);
        free(symbol);
        return NULL;
    }
    strcpy(symbol->name, symbol_name);

    /* Connect the symbol to the symbol list */
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

/**
 * freeSymbolList
 * Frees the symbol list from the memory.
 * @param head The head of the symbol list.
 */
void freeSymbolList(SymbolNode *head) {
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