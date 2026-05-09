#include "first_pass.h"

unsigned int data_image[4096];
unsigned int code_image[4096];
char are_image[4096] = "";

static int getAddressMode(char *operand) {
    if (operand[0] == '#')
        return 0;
    else if (operand[0] == '%')
        return 2;
    else if ((operand[0] == 'r' && (operand[1] >= '0' && operand[1] <= '7')) && (isspace(operand[2]) || operand[2] == '\0'))
        return 3;
    else
        return 1;
}

static int processData(char *operands, int *dc, int *ic, int line_number, int *error_flag){
    char *data;
    char *errptr;
    long int number;

    data = strtok(operands, ",");
    while (data != NULL) {
        number = strtol(data, &errptr, 10);
        if (errptr == data) {
            fprintf(stderr, "ERROR at line %d: No number inputed after .data.\n", line_number);
            *error_flag = 1;
            return 1;
        }
        while (*errptr != '\0' && isspace((unsigned char) *errptr))
            errptr++;
        
        if (*errptr != '\0') {
            fprintf(stderr, "ERROR at line %d: Invalid characters or missing comma between numbers.\n", line_number);
            *error_flag = 1;
            return 1;
        }
        
        data_image[*dc] = number & 0xFFF;
        (*dc)++;
        CHECK_MEM(*dc, *ic);
        
        if ((*dc) >= 4096) {
            fprintf(stderr, "FATAL ERROR: memory overflow\n");
            return -1;
        }
        data = strtok(NULL, ",");
    }
    return 0;
}

static int processString(char *operands, int *dc, int *ic, int line_number, int *error_flag) {
    if (operands[0] != '"') {
        fprintf(stderr, "ERROR at line %d: String declaration must start with \".\n", line_number);
        *error_flag = 1;
        return 1;
    }
    
    operands++;
    while (*operands != '\0' && *operands != '"') {
        CHECK_MEM(*dc, *ic);
        data_image[*dc] = (unsigned int) *operands; 
        operands++;
        (*dc)++;
    }
    CHECK_MEM(*dc, *ic);
    data_image[*dc] = (unsigned int) '\0';
    (*dc)++;
    CHECK_MEM(*dc, *ic);
    
    if (*operands == '\0') {
        fprintf(stderr, "ERROR at line %d: String wasn't closed with \".\n", line_number);
        *error_flag = 1;
        return 1;
    }
    operands++;
    
    while (*operands != '\0' && isspace((unsigned char) *operands)){
        operands++;
    }
    if (*operands != '\0') {
        fprintf(stderr, "ERROR at line %d: String declaration must not have extra characters after the string.\n", line_number);
        *error_flag = 1;
        return 1;
    }
    return 0;
}

static int processExtern(SymbolNode **head, SymbolNode **tail, SymbolNode **symbol ,char *operands, int line_number, int *error_flag) {
    char symbol_name[SYMBOL_NAME_MAX_LENGTH] = "";
    char extra[2] = "";
    int symbol_validity = 0;
    int word_count = sscanf(operands, "%s %1s", symbol_name, extra);

    if (word_count < 1) {
        fprintf(stderr, "ERROR at line %d: Missing symbol name after .extern.\n", line_number);
        *error_flag = 1;
        return 1;
    }
    if (word_count > 1) {
        fprintf(stderr, "ERROR at line %d: Extra text after symbol name in .extern declaration.\n", line_number);
        *error_flag = 1;
        return 1;
    }
    symbol_validity = processSymbol(head, tail, symbol, symbol_name, line_number, error_flag, 1);
    if (symbol_validity == -1) {
        return -1;
    }
    if (symbol_validity == 1) {
        return 1;
    }

    (*symbol)->is_extern = 1;
    return 0;
}

static int processCommand(char *current_command, char *operands, int *dc, int *ic, int line_number, int *error_flag) {
    int command_index = -1;
    int operand_count = 0;
    int command_word_count = 0;
    int source_mode = -1;
    int dest_mode = -1;
    int final_source = 0;
    int final_dest = 0;
    int offset = 1;
    long int number = 0;
    unsigned int binary_command = 0;
    char *parsed_operands[4] = {NULL};
    char *source_str = NULL;
    char *dest_str = NULL;
    char *token;

    command_index = findCommand(current_command);
    if (command_index == -1) {
        fprintf(stderr, "ERROR at line %d: Invalid command %s.\n", line_number, current_command);
        *error_flag = 1;
        return 1;
    }
    
    token = strtok(operands, ",");
    while (token != NULL && operand_count < 4) {
        while (*token != '\0' && isspace((unsigned char) *token))
            token++;
        parsed_operands[operand_count] = token;
        operand_count++;
        token = strtok(NULL, ",");
    }

    command_word_count = command_table[command_index].operand_count;

    if (operand_count != command_word_count) {
        fprintf(stderr, "ERROR at line %d: Command %s takes %d operands and not %d.\n", line_number, current_command, command_word_count, operand_count);
        *error_flag = 1;
        return 1;
    }
    
    if (operand_count == 2) {
        source_mode = getAddressMode(parsed_operands[0]);
        source_str = parsed_operands[0];
        dest_mode = getAddressMode(parsed_operands[1]);
        dest_str = parsed_operands[1];
    }
    if (operand_count == 1) {
        source_mode = -1;
        dest_mode = getAddressMode(parsed_operands[0]);
        dest_str = parsed_operands[0];
    }
    if (operand_count == 0) {
        source_mode = -1;
        dest_mode = -1;
    }

    if (source_mode != -1) {
        if ((command_table[command_index].source_modes & (1 << source_mode)) == 0) {
            fprintf(stderr, "ERROR at line %d: Invalid source addressing mode for command %s.\n", line_number, current_command);
            *error_flag = 1;
            return 1;
        }
        final_source = source_mode;
    }
    
    if (dest_mode != -1) {
        if ((command_table[command_index].dest_modes & (1 << dest_mode)) == 0) {
            fprintf(stderr, "ERROR at line %d: Invalid destination addressing mode for command %s.\n", line_number, current_command);
            *error_flag = 1;
            return 1;
        }
        final_dest = dest_mode;
    }

    /* Save the command */
    binary_command = (command_table[command_index].opcode << 8) |
                     (command_table[command_index].funct << 4) |
                     (final_source << 2) |
                     (final_dest);

    code_image[(*ic) - 100] = binary_command;
    are_image[(*ic) - 100] = 'A';

    offset = 1;
    if (operand_count == 2) {
        if (source_mode == 0) {
            number = atoi(source_str + 1);
            code_image[(*ic) - 100 + offset] = number & 0xFFF;
            are_image[(*ic) - 100 + offset] = 'A';
        }
        else if (source_mode == 3) {
            number = 1 << (source_str[1] - '0');
            code_image[(*ic) - 100 + offset] = number;
            are_image[(*ic) - 100 + offset] = 'A';
        }
        else {
            code_image[(*ic) - 100 + offset] = 0;
        }
        offset++;
    }

    if (operand_count >= 1) {
        if (dest_mode == 0) {
            number = atoi(dest_str + 1);
            code_image[(*ic) - 100 + offset] = number & 0xFFF;
            are_image[(*ic) - 100 + offset] = 'A';
        }
        else if (dest_mode == 3) {
            number = 1 << (dest_str[1] - '0');
            code_image[(*ic) - 100 + offset] = number;
            are_image[(*ic) - 100 + offset] = 'A';
        }
        else {
            code_image[(*ic) - 100 + offset] = 0;
        }
    }

    (*ic) += (1 + operand_count);
    CHECK_MEM(*dc, *ic);

    return 0;
}

int firstPass(FILE *file, char *file_name) {
    char line[MAX_LINE_LENGTH] = "";
    char symbol_name[SYMBOL_NAME_MAX_LENGTH] = "";
	char first_word[MAX_LINE_LENGTH] = "";
    char second_word[MAX_LINE_LENGTH] = "";
    char third_word[MAX_LINE_LENGTH] = "";
    char extra[2] = "";
    char *parsed_operands[4] = "";
    char *current_command;
    char *operands;
    char *source_str = NULL;
    char *dest_str = NULL;
    char *data;
    char *token;
    char *ptr;
    char *errptr;

	int error_flag = 0;
    int error_type = 0;
    int in_label = 0;
    int ic = 100;
    int dc = 0;
    int line_number = 0;
    int symbol_validity = 1;
    int word_count = 0;
    unsigned int binary_command = 0;

    SymbolNode *symbol;
    SymbolNode *current;
    SymbolNode *head = NULL;
    SymbolNode *tail = NULL;

    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        in_label = 0;

		word_count = sscanf(line, "%s %s %s %1s", first_word, second_word, third_word, extra);

        if (strlen(first_word) > 1 && first_word[strlen(first_word) - 1] == ':') {
            first_word[strlen(first_word) - 1] = '\0'; /* Chop off the colon. */
            strcpy(symbol_name, first_word); /* Save the symbol name. */
            in_label = 1;

            if (strcmp(second_word, ".extern") == 0 || strcmp(second_word, ".entry") == 0) {
                fprintf(stderr, "WARNING at line %d: Labels declared with .extern or .entry will be ignored.\n", line_number);
            }
            else {
                /* Check symbol validity and add to symbol table */
                symbol_validity = processSymbol(&head, &tail, &symbol, symbol_name, line_number, &error_flag, 0);
                if (symbol_validity == -1) {
                    freeSymbolList(head);
                    return 1;
                }
                if (symbol_validity == 1) {
                    continue;
                }

                if (strcmp(second_word, ".data") == 0 || strcmp(second_word, ".string") == 0) {
                    symbol->value = dc;
                    symbol->is_data = 1;
                }
                else {
                    symbol->value = ic;
                    symbol->is_code = 1;
                }
            }
        }
        /* handle commands*/
        if (in_label)
            current_command = second_word;
        else
            current_command = first_word;
        /* Set a pointer to the command and then set the operands to be the part of the string after current_command. */
        ptr = strstr(line, current_command);
        
        if (ptr != NULL) {
            operands = ptr + strlen(current_command); 
        }
        /* Skip whitespace */
        while (*operands != '\0' && isspace((unsigned char) *operands))
            operands++;
        
        if (strcmp(current_command, ".data") == 0) {
            error_type = processData(operands, &dc, &ic, line_number, &error_flag);
            if (error_type == -1) {
                freeSymbolList(head);
                return 1;
            }
            if (error_type == 1) {
                continue;
            }
        }
        else if (strcmp(current_command, ".entry") == 0) {
            /* .entry is handled in the second pass */
            continue;
        }
        else if (strcmp(current_command, ".extern") == 0) {
            error_type = processExtern(&head, &tail, &symbol, operands, line_number, &error_flag);
            if (error_type == -1) {
                freeSymbolList(head);
                return 1;
            }
            if (error_type == 1)
                continue;
        }
        else if (strcmp(current_command, ".string") == 0) {
            error_type = processString(operands, &dc, &ic, line_number, &error_flag);
            if (error_type == -1) {
                freeSymbolList(head);
                return 1;
            }
            if (error_type == 1) {
                continue;
            }
        }
        else {
            error_type = processCommand(current_command, operands, &dc, &ic, line_number, &error_flag);
            if (error_type == -1) {
                freeSymbolList(head);
                return 1;
            }
            if (error_type == 1) {
                continue;
            }
        }
    }
    current = head;
    while (current != NULL) {
        if (current->is_data == 1)
            current->value += ic;
        current = current->next;
    }
    if (error_flag == 1) {
        freeSymbolList(head);
        return 1;
    }
    return 0;
}