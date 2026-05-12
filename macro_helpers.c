#include "macro_helpers.h"

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
int isMacroValid(char *macro_name, MacroNode *mac_head, SymbolNode *sym_head, int line_number) {
    int index = 0;
    int num_reserved_words = ARRAY_SIZE(reserved_words);

    /* Check if the macro starts with an english letter and has only numbers, underscores or letters inside the name */
    if (!isalpha(macro_name[0])) {
        fprintf(stderr, "ERROR at line %d: Macro name must start with an English letter.\n", line_number);
        return 1;
    }
    while(index < strlen(macro_name)) {
        if (!isalnum(macro_name[index]) && macro_name[index] != '_') {
            fprintf(stderr, "ERROR at line %d: Macro name must only have English letters, numbers or an underscore.\n", line_number);
            return 1;
        }
        index++;
    }
    index = 0;
    
    while(index < num_reserved_words) {
        if (strcmp(macro_name, reserved_words[index]) == 0) {
            fprintf(stderr, "ERROR at line %d: Macro shares the name with a reserved word.\n", line_number);
            return 1;
        }
        index++;
    }

    if (findMacro(macro_name, mac_head) != NULL) {
        fprintf(stderr, "ERROR at line %d: A macro with an identical name was found.\n", line_number);
        return 1;
    }
    if (findSymbol(macro_name, sym_head) != NULL) {
        fprintf(stderr, "ERROR at line %d: A macro and symbol with identical names were found.\n", line_number);
        return 1;
    }
    
    return 0; /* The macro is a valid name. */
}

/**
 * findMacro
 * Finds the macro node from its name.
 * @param macro_name The macro name.
 * @param head The head of the macro list.
 * @return NULL if the macro wasn't found or it returns the pointer to the macro node if it was found.
 */
MacroNode *findMacro(char *macro_name, MacroNode *head) {
    MacroNode *node = head;

    while(node != NULL) {
        if (strcmp(node->name, macro_name) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL; /* The macro wasn't found. */
}

/**
 * expandMacro
 * Expands the macro into code and outputs it into the .am file
 * @param output The output .am file.
 * @param macro The macro node that has the code.
 */
void expandMacro(FILE *output, MacroNode *macro) {
    int index = 0;

    while (index < macro->line_count) {
        fprintf(output, "%s", macro->code[index]);
        index++;
    }
}

/**
 * freeMacroList
 * Frees the macro list from the memory.
 * @param head The head of the macro list.
 */
void freeMacroList(MacroNode *head) {
    int index = 0;
    MacroNode *node = head;
    MacroNode *next_node;

    while (node != NULL) {
        next_node = node->next;
        /* Free the code of the macro node. */
        if (node->code != NULL) {
            for (index = 0; index < node->line_count; index++)
                free(node->code[index]);

            free(node->code);
        }
        /* Free the name of the macro. */
        if (node->name != NULL)
            free(node->name);

        free(node);
        node = next_node;
    }
}