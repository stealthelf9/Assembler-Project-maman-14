#include "macro_helpers.h"

int isMacroValid(char *macro_name, MacroNode *mac_head, SymbolNode *sym_head, int line_number) {
    int index = 0;
    int num_reserved_words = ARRAY_SIZE(reserved_words);

    MacroNode *node = mac_head;

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
    return 0;
}

MacroNode *findMacro(char *macro_name, MacroNode *head) {
    MacroNode *node = head;

    while(node != NULL) {
        if (strcmp(node->name, macro_name) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void expandMacro(FILE *output, MacroNode *macro) {
    int index = 0;

    while (index < macro->line_count) {
        fprintf(output, "%s", macro->code[index]);
        index++;
    }
}

void freeMacroList(MacroNode *head) {
    int i;
    MacroNode *node = head;
    MacroNode *next_node;

    while (node != NULL) {
        next_node = node->next;

        if (node->code != NULL) {
            for (i = 0; i < node->line_count; i++)
                free(node->code[i]);

            free(node->code);
        }

        if (node->name != NULL)
            free(node->name);

        free(node);
        node = next_node;
    }
}