#include "pre_assembler.h"

/* Processes labels, .extern, and .entry to check for macro conflicts 
   because the macro cannot have the same name as a symbol */
static int processPreAssemblerSymbol(char *first_word, char *second_word, int word_count, 
                              MacroNode *mac_head, SymbolNode **sym_head, SymbolNode **sym_tail, 
                              int line_number, int *error_flag) {
    char symbol_name[SYMBOL_NAME_MAX_LENGTH] = "";
    int is_symbol = 0;

    if ((strcmp(first_word, ".extern") == 0 || strcmp(first_word, ".entry") == 0) && word_count >= 2) {
        if (strlen(second_word) > SYMBOL_NAME_MAX_LENGTH - 2) {
            fprintf(stderr, "ERROR at line %d: Symbol name is longer than 31 characters.\n", line_number);
            *error_flag = 1;
            return 1;
        }
        strcpy(symbol_name, second_word);
        is_symbol = 1;
    }
    else if (strlen(first_word) > 1 && first_word[strlen(first_word) - 1] == ':') {
        if (strlen(first_word) > SYMBOL_NAME_MAX_LENGTH - 1) {
            fprintf(stderr, "ERROR at line %d: Symbol name is longer than 31 characters.\n", line_number);
            *error_flag = 1;
            return 1;
        }
        strcpy(symbol_name, first_word);
        symbol_name[strlen(symbol_name) - 1] = '\0'; /* Chop off colon */
        is_symbol = 1;
    }

    if (is_symbol == 1) {
        if (findMacro(symbol_name, mac_head) != NULL) {
            fprintf(stderr, "ERROR at line %d: A macro and symbol with identical names were found.\n", line_number);
            *error_flag = 1;
            return 1;
        }
        /* If setSymbol fails due to memory, it returns NULL */
        if (setSymbol(symbol_name, sym_head, sym_tail, line_number) == NULL) {
            return -1; 
        }
    }
    return 0;
}

/* Safely allocates memory to add a new line of code to the current macro */
static int addLineToMacro(MacroNode *macro, char *line, int line_number) {
    macro->code = realloc(macro->code, (macro->line_count + 1) * sizeof(char *));
    if (macro->code == NULL) {
        fprintf(stderr, "ERROR at line %d: Out of memory.\n", line_number);
        return -1;
    }
    macro->code[macro->line_count] = calloc(1, strlen(line) + 1);
    if (macro->code[macro->line_count] == NULL) {
        fprintf(stderr, "ERROR at line %d: Out of memory.\n", line_number);
        return -1;
    }
    strcpy(macro->code[macro->line_count], line);
    macro->line_count++;
    return 0;
}

/* 3. Validates the macro name and allocates the initial struct */
static int startMacroDefinition(char *macro_name, int macro_param_count, 
                         MacroNode **mac_head, MacroNode **mac_tail, SymbolNode *sym_head, 
                         int line_number, int *error_flag, MacroNode **current_macro) {
    if (macro_param_count == 1) {
        if (isMacroValid(macro_name, *mac_head, sym_head, line_number) == 1) {
            *error_flag = 1;
            return 1;
        }
        
        *current_macro = calloc(1, sizeof(MacroNode));
        if (*current_macro == NULL) {
            fprintf(stderr, "ERROR at line %d: Out of memory.\n", line_number);
            return -1;
        }
        
        (*current_macro)->name = calloc(1, strlen(macro_name) + 1);
        if ((*current_macro)->name == NULL) {
            fprintf(stderr, "ERROR at line %d: Out of memory.\n", line_number);
            free(*current_macro);
            return -1;
        }
        
        strcpy((*current_macro)->name, macro_name);
        (*current_macro)->next = NULL;

        if (*mac_head == NULL) {
            *mac_head = *current_macro;
            *mac_tail = *current_macro;
        } else {
            (*mac_tail)->next = *current_macro;
            *mac_tail = *current_macro;
        }
        return 0;
    }
    else if (macro_param_count == 2) {
        fprintf(stderr, "ERROR at line %d: Too many parameters after mcro command.\n", line_number);
        *error_flag = 1;
        return 1;
    }
    else {
        fprintf(stderr, "ERROR at line %d: No macro name given.\n", line_number);
        *error_flag = 1;
        return 1;
    }
}

int preAssembler(FILE *file, char *file_name) {
    char line[MAX_LINE_LENGTH] = "";
    char macro_name[MACRO_NAME_MAX_LENGTH] = "";
    char extra[2] = ""; /* We do size 2 in order to be able to use it with sscanf to skip whitespaces */
    char first_word[MAX_LINE_LENGTH] = "";
    char second_word[MAX_LINE_LENGTH] = "";
    char new_file_name[MAX_FILE_NAME_LENGTH] = "";

    int c;
    int macro_param_count;
    int word_count;
    int in_macro = 0;
    int error_flag = 0;
    int line_number = 0;
    int mac_status = 0;
    int sym_status = 0;
    

    MacroNode *macro;
    MacroNode *mac_head = NULL;
    MacroNode *mac_tail = NULL;

    SymbolNode *sym_head = NULL;
    SymbolNode *sym_tail = NULL;

    FILE *output;

    if (file == NULL) {
        fprintf(stderr, "ERROR: Failed to open file.\n");
        return 1;
    }
    
    /* Build the .am file name. */
    sprintf(new_file_name, "%s.am", file_name);

    output = fopen(new_file_name, "w");
    if (output == NULL) {
        fprintf(stderr, "ERROR at line %d: Error creating .am file.\n", line_number);
        remove(new_file_name);
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;

        /* Wipe the strings clean for this new line */
        macro_name[0] = '\0';
        first_word[0] = '\0';
        second_word[0] = '\0';
        /* If the line is too long */
        if (strchr(line, '\n') == NULL && !feof(file)) {
            fprintf(stderr, "ERROR at line %d: Line length is longer than 80 characters\n", line_number);
            /* Read and discard characters until a newline or EOF is found */
            while ((c = fgetc(file)) != '\n' && c != EOF);
            error_flag = 1;
            continue;
        }
        
        /* If the macro name is too long */
        macro_param_count = sscanf(line, " mcro %32s %1s", macro_name, extra);
        if (strlen(macro_name) == SYMBOL_NAME_MAX_LENGTH - 1) {
            fprintf(stderr, "ERROR at line %d: Macro name length is longer than 31 characters.\n", line_number);
            error_flag = 1;
            continue;
        }

        word_count = sscanf(line, "%s %32s", first_word, second_word);

        /* Skip comment lines and completely empty lines */
        if (line[0] == ';' || first_word[0] == ';' || word_count <= 0)
            continue;
        /* If we're in the symbol declaration */
        sym_status = processPreAssemblerSymbol(first_word, second_word, word_count, mac_head, &sym_head, &sym_tail, line_number, &error_flag);
        /* Fatal memory error */
        if (sym_status == -1) {
            freeMacroList(mac_head);
            freeSymbolList(sym_head);
            fclose(output);
            remove(new_file_name);
            return 1;
        }
        if (sym_status == 1) continue; /* Syntax error, move to next line */
        /* If we find mcro end then the macro is finished.*/
        if (word_count == 1 && strcmp(first_word, "mcroend") == 0) {
            in_macro = 0;
            continue;
        }
        /* If mcroend line has more than just mcroend, then throw an error and check the next file. */
        else if (word_count == 2 && strcmp(first_word, "mcroend") == 0) {
            fprintf(stderr, "ERROR at line %d: Too many parameters after mcroend command.\n", line_number);
            error_flag = 1;
            continue;
        }
        /* If we're in the code of the macro then save the code. */
        else if (in_macro == 1) {
            if (addLineToMacro(macro, line, line_number) == -1) {
                freeMacroList(mac_head);
                freeSymbolList(sym_head);
                fclose(output);
                remove(new_file_name);
                return 1;
            }
            continue;
        }
        /* If it's not a macro declaration, a blank line or a comment line, then write the line and continue to the next one. */
        else if ((macro_param_count == EOF || macro_param_count == 0) && strcmp(first_word, "mcro") != 0) {
            /* If the word is a macro. */
            macro = findMacro(first_word, mac_head);
            if (word_count == 1 && macro != NULL ) {
                expandMacro(output, macro);
                continue;
            }
            /* Else just write the line normally. */
            fprintf(output, "%s", line);
            continue;
        }
        /* If it's a macro and it has a valid name, then perform the logic of saving the macro to the struct. */
        else {
            mac_status = startMacroDefinition(macro_name, macro_param_count, &mac_head, &mac_tail, sym_head, line_number, &error_flag, &macro);
            if (mac_status == -1) {
                freeMacroList(mac_head);
                freeSymbolList(sym_head);
                fclose(output);
                remove(new_file_name);
                return 1;
            }
            if (mac_status == 1) continue;
            in_macro = 1;
            continue;
        }
    }
    freeMacroList(mac_head);
    freeSymbolList(sym_head);
    fclose(output);
    if (error_flag == 1) {
        remove(new_file_name);
        return 1;
    }
    return 0;
}