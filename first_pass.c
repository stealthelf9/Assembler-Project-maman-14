#include "first_pass.h"

/* Checks if there's a comma before the operand, 2 commas in a row or if there's a comma in the end. */
static int checkCommas(char *str, int line_number, int *error_flag) {
    int index;
    int has_data = 0;
    int last_was_comma = 0;

    if (str == NULL || str[0] == '\0')
        return 0; /* processData and processString will deal with empty operands. */

    for (index = 0; str[index] != '\0'; index++) {
        /* Skip spaces */
        if (isspace((unsigned char)str[index]))
            continue;

        if (str[index] == ',') {
            if (!has_data || last_was_comma) {
                fprintf(stderr, "ERROR at line %d: Illegal comma placement (leading or consecutive commas).\n", line_number);
                *error_flag = 1;
                return 1;
            }
            last_was_comma = 1;
        }
        else {
            has_data = 1;
            last_was_comma = 0;
        }
    }

    /* If the loop finished and the last non-space character is a comma */
    if (last_was_comma) {
        fprintf(stderr, "ERROR at line %d: Illegal trailing comma.\n", line_number);
        *error_flag = 1;
        return 1;
    }

    return 0;
}

/* Processes the .data command and saves numbers into the .data image. */
static int processData(char *operands, int *ic, int *dc, unsigned int *data_image, int line_number, int *error_flag){
    char *data;
    char *errptr;
    long int number;

    if (operands == NULL || operands[0] == '\0') {
        fprintf(stderr, "ERROR at line %d: Missing operands after .data directive.\n", line_number);
        *error_flag = 1;
        return 1;
    }

    if (checkCommas(operands, line_number, error_flag) == 1)
        return 1;

    /* Get the operands and check if a number was inputted */
    data = strtok(operands, ",");
    while (data != NULL) {
        number = strtol(data, &errptr, 10);
        if (errptr == data) {
            fprintf(stderr, "ERROR at line %d: No number inputed after .data.\n", line_number);
            *error_flag = 1;
            return 1;
        }
        /* Skip whitespaces to make sure that the error of where the number ends is only whitespace and that there was no comma missing between numbers. */
        while (*errptr != '\0' && isspace((unsigned char) *errptr))
            errptr++;
        
        if (*errptr != '\0') {
            fprintf(stderr, "ERROR at line %d: Invalid characters or missing comma between numbers.\n", line_number);
            *error_flag = 1;
            return 1;
        }
        /* Mask the number so that only 3 digits/letters will be used in the hexadecimal representation */
        data_image[*dc] = number & MASK_12_BITS;
        (*dc)++;
        CHECK_MEM(*dc, *ic);
        
        data = strtok(NULL, ",");
    }
    return 0;
}

/* Processes the .string command, converts strings into integers and saves them into the data_image. */
static int processString(char *operands, int *ic, int *dc, unsigned int *data_image, int line_number, int *error_flag) {
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
    
    /* Skip whitespace */
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

/* Processes the .extern command and saves the extern symbol to the symbol table. */
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
        return -1; /* Fatal error with memory */
    }
    if (symbol_validity == 1) {
        return 1; /* Symbol has an invalid name */
    }

    (*symbol)->is_extern = 1;
    return 0;
}

/* Processes every command (e.g. mov, prn, etc.), assigns the opcode, sets the binary number and saves it into the code image. */
static int processCommand(char *current_command, char *operands, char *are_image, int *ic, int *dc, unsigned int *code_image, int line_number, int *error_flag) {
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
    
    if (checkCommas(operands, line_number, error_flag) == 1)
        return 1;

    /* Get the first operand, then skip whitespaces then save the next operands into parsed_operands. */
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
    /* If there are 2 operands, then there's a source and destination mode */
    if (operand_count == 2) {
        source_mode = getAddressMode(parsed_operands[0]);
        source_str = parsed_operands[0];
        dest_mode = getAddressMode(parsed_operands[1]);
        dest_str = parsed_operands[1];
    }
    /* if there's only 1 operand then there's only a destination mode */
    else if (operand_count == 1) {
        source_mode = -1;
        dest_mode = getAddressMode(parsed_operands[0]);
        dest_str = parsed_operands[0];
    }
    else if (operand_count == 0) {
        source_mode = -1;
        dest_mode = -1;
    }

    if (source_mode != -1) {
        /* If the source mode isn't a valid source mode of the command */
        if ((command_table[command_index].source_modes & (1 << source_mode)) == 0) {
            fprintf(stderr, "ERROR at line %d: Invalid source addressing mode for command %s.\n", line_number, current_command);
            *error_flag = 1;
            return 1;
        }
        /* Otherwise, set the final source mode to be the source mode (so that we won't have to deal with the source mode being -1 and final_source is set by default to 0). */
        final_source = source_mode;
    }
    /* Same thing for destination mode */
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
    /* Save it to the images */
    code_image[(*ic) - IC_INIT_VALUE] = binary_command;
    are_image[(*ic) - IC_INIT_VALUE] = 'A';

    offset = 1;
    /* Save the number in the source mode (other modes will be dealt with in the second pass)*/
    if (operand_count == 2) {
        if (source_mode == 0) {
            number = atoi(source_str + 1);
            code_image[(*ic) - IC_INIT_VALUE + offset] = number & MASK_12_BITS;
            are_image[(*ic) - IC_INIT_VALUE + offset] = 'A';
        }
        else if (source_mode == 3) {
            number = 1 << (source_str[1] - '0'); /* We get the value of the string by removing the ascii value of 0 from the ascii value of the wanted number. */
            code_image[(*ic) - IC_INIT_VALUE + offset] = number;
            are_image[(*ic) - IC_INIT_VALUE + offset] = 'A';
        }
        else {
            code_image[(*ic) - IC_INIT_VALUE + offset] = 0;
        }
        offset++;
    }
    /* Same thing for the destination mode */
    if (operand_count >= 1) {
        if (dest_mode == 0) {
            number = atoi(dest_str + 1);
            code_image[(*ic) - IC_INIT_VALUE + offset] = number & MASK_12_BITS;
            are_image[(*ic) - IC_INIT_VALUE + offset] = 'A';
        }
        else if (dest_mode == 3) {
            number = 1 << (dest_str[1] - '0');
            code_image[(*ic) - IC_INIT_VALUE + offset] = number;
            are_image[(*ic) - IC_INIT_VALUE + offset] = 'A';
        }
        else {
            code_image[(*ic) - IC_INIT_VALUE + offset] = 0;
        }
    }

    (*ic) += (1 + operand_count);
    CHECK_MEM(*dc, *ic);

    return 0;
}

/**
 * firstPass
 * This is the first pass. It uses the file processed by the pre-assembler, it saves the commands into the code image, strings and numbers into the data image
 * and also saves the binary values of the commands that only have immediate or register modes inside the commands into the code image and ARE image.
 * @param file The processed file from the pre-assembler.
 * @param file_name The file name.
 * @param are_image The ARE image.
 * @param code_image The code image.
 * @param data_image The data image.
 * @param ic The instruction counter.
 * @param dc The data counter.
 * @param head The head of the symbol table.
 * @return 1 for faliure, 0 for success. 
 */
int firstPass(FILE *file, char *file_name, char *are_image, unsigned int *code_image, unsigned int *data_image, int *ic, int *dc, SymbolNode **head) {
    char line[MAX_LINE_LENGTH] = "";
    char symbol_name[SYMBOL_NAME_MAX_LENGTH] = "";
	char first_word[MAX_LINE_LENGTH] = "";
    char second_word[MAX_LINE_LENGTH] = "";
    char third_word[MAX_LINE_LENGTH] = "";
    char extra[2] = "";
    char *current_command;
    char *operands;
    char *ptr;

	int error_flag = 0;
    int error_type = 0;
    int in_label = 0;
    int line_number = 0;
    int symbol_validity = 1;
    int word_count = 0;

    SymbolNode *symbol;
    SymbolNode *current;
    SymbolNode *tail = NULL;

    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        in_label = 0;

		word_count = sscanf(line, "%s %s %s %1s", first_word, second_word, third_word, extra);
        /* If it's a symbol declaration */
        if (strlen(first_word) > 1 && first_word[strlen(first_word) - 1] == ':') {
            first_word[strlen(first_word) - 1] = '\0'; /* Chop off the colon. */
            strcpy(symbol_name, first_word); /* Save the symbol name. */
            in_label = 1;

            if (strcmp(second_word, ".extern") == 0 || strcmp(second_word, ".entry") == 0) {
                /* .extern and .entry will be handeled as a command that will be the second word in the handel commands section */
                fprintf(stderr, "WARNING at line %d: Labels declared with .extern or .entry will be ignored.\n", line_number);
            }
            else {
                /* Check symbol validity and add to symbol table */
                symbol_validity = processSymbol(head, &tail, &symbol, symbol_name, line_number, &error_flag, 0);
                if (symbol_validity == -1) {
                    freeSymbolList(*head);
                    return 1;
                }
                if (symbol_validity == 1) {
                    continue;
                }

                if (strcmp(second_word, ".data") == 0 || strcmp(second_word, ".string") == 0) {
                    symbol->value = *dc;
                    symbol->is_data = 1;
                }
                else {
                    symbol->value = *ic;
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
        /* Skip whitespaces */
        while (*operands != '\0' && isspace((unsigned char) *operands))
            operands++;
        
        if (strcmp(current_command, ".data") == 0) {
            error_type = processData(operands, ic, dc, data_image, line_number, &error_flag);
            if (error_type == -1) { /* Fatal error */
                freeSymbolList(*head);
                return 1;
            }
            if (error_type == 1) {
                continue;
            }
        }

        /* .entry is handled in the second pass */        
        else if (strcmp(current_command, ".entry") == 0)
            continue;
        
        else if (strcmp(current_command, ".extern") == 0) {
            error_type = processExtern(head, &tail, &symbol, operands, line_number, &error_flag);
            if (error_type == -1) { /* Fatal error */
                freeSymbolList(*head);
                return 1;
            }
            if (error_type == 1)
                continue;
        }
        else if (strcmp(current_command, ".string") == 0) {
            error_type = processString(operands, ic, dc, data_image, line_number, &error_flag);
            if (error_type == -1) { /* Fatal error */
                freeSymbolList(*head);
                return 1;
            }
            if (error_type == 1) {
                continue;
            }
        }
        else {
            error_type = processCommand(current_command, operands, are_image, ic, dc, code_image, line_number, &error_flag);
            if (error_type == -1) {
                freeSymbolList(*head);
                return 1;
            }
            if (error_type == 1) {
                continue;
            }
        }
    }
    /* The data block of the imaginary computer is after the code block. So add the ic to the value of every .data symbol. */
    current = *head;
    while (current != NULL) {
        if (current->is_data == 1)
            current->value += *ic;
        current = current->next;
    }
    if (error_flag == 1) {
        freeSymbolList(*head);
        return 1;
    }
    return 0;
}