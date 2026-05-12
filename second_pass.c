#include "second_pass.h"

static int processEntry(SymbolNode *head, char *operands, int line_number, int *error_flag) {
    char symbol_name[SYMBOL_NAME_MAX_LENGTH] = "";
    char extra[2] = "";

    int word_count = sscanf(operands, "%s %1s", symbol_name, extra);

    SymbolNode *symbol;

    if (word_count < 1) {
        fprintf(stderr, "ERROR at line %d: Missing symbol name after .entry.\n", line_number);
        *error_flag = 1;
        return 1;
    }
    if (word_count > 1) {
        fprintf(stderr, "ERROR at line %d: Extra text after symbol name in .entry declaration.\n", line_number);
        *error_flag = 1;
        return 1;
    }
    
    symbol = findSymbol(symbol_name, head);
    if (symbol == NULL) {
        fprintf(stderr, "ERROR at line %d: Symbol %s was never declared.\n", line_number, symbol_name);
        *error_flag = 1;
        return 1;
    }
    if (symbol->is_extern == 1) {
        fprintf(stderr, "ERROR at line %d: Symbol %s cannot be both .extern and .entry.\n", line_number, symbol_name);
        *error_flag = 1;
        return 1;
    }
    symbol->is_entry = 1;
    return 0;
}

static int exportObFile(char *file_name, char *are_image, int ic, int dc, unsigned int *code_image, unsigned int *data_image) {
    char new_file_name[MAX_FILE_NAME_LENGTH];
    int index = 0;
    FILE *output;

    /* Create the new file name. */
    sprintf(new_file_name, "%s.ob", file_name);

    output = fopen(new_file_name, "w");
    if (output == NULL) {
        fprintf(stderr, "ERROR: Error creating .ob file.\n");
        remove(new_file_name);
        return 1;
    }

    fprintf(output, "%d %d\n", (ic - IC_INIT_VALUE), dc);
    /* Print the code */
    for (index = 0; index < (ic - IC_INIT_VALUE); index++)
        fprintf(output, "%04d %03X %c\n", (index + IC_INIT_VALUE), code_image[index], are_image[index]);

    /* Print the data */
    for (index = 0; index < dc; index++)
        fprintf(output, "%04d %03X A\n", (ic + index), data_image[index]); /* Every data image file*/
    
    /* Close the output */
    fclose(output);
    return 0;
}

static int exportEntFile(char *file_name, SymbolNode *head) {
    char new_ent_file_name[MAX_FILE_NAME_LENGTH];
    SymbolNode *symbol = head;
    FILE *ent_output = NULL;

    sprintf(new_ent_file_name, "%s.ent", file_name);

    while (symbol != NULL) {
        if (symbol->is_entry == 1) {
            /* We found an entry. Open the file if it isn't open yet. */
            if (ent_output == NULL) {
                ent_output = fopen(new_ent_file_name, "w");
                if (ent_output == NULL) {
                    fprintf(stderr, "ERROR: Error creating %s file.\n", new_ent_file_name);
                    return 1;
                }
            }
            /* Print the name and its final value from the symbol table */
            fprintf(ent_output, "%s %04d\n", symbol->name, symbol->value);
        }
        symbol = symbol->next;
    }

    /* If we ever opened the file, close it. */
    if (ent_output != NULL) {
        fclose(ent_output);
    }
    return 0;
}

/**
 * secondPass 
 * This is the second pass of the assembler. It fills in the the images that are from other modes that are from the symbol table created by the first pass.
 * @param file The processed file from the pre-assembler.
 * @param file_name The file name.
 * @param are_image The image for all the A R E letters.
 * @param code_image The code image.
 * @param data_image The data image.
 * @param dc The data counter.
 * @param head The head to the linked list of the symbols.
 * @return 1 for faliure, 0 for success. 
 */
int secondPass(FILE *file, char *file_name, char *are_image, unsigned int *code_image, unsigned int *data_image, int dc, SymbolNode *head) {
    char line[MAX_LINE_LENGTH] = "";
	char first_word[MAX_LINE_LENGTH] = "";
    char second_word[MAX_LINE_LENGTH] = "";
    char third_word[MAX_LINE_LENGTH] = "";
    char new_ext_file_name[MAX_FILE_NAME_LENGTH];    
    char extra[2] = "";
    char *parsed_operands[4] = {NULL};
    char *symbol_name;
    char *current_command;
    char *operands;
    char *token;
    char *ptr;

	int error_flag = 0;
    int error_type = 0;
    int in_label = 0;
    int ic = IC_INIT_VALUE;
    int line_number = 0;
    int operand_count = 0;
    int address_mode[2] = {0};
    int offset = 0;
    int index = 0;
    int export_ob = 1;
    int export_ent = 1;
    
    SymbolNode *symbol;

    FILE *ext_output = NULL;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        in_label = 0;
        /* Save the words */
		sscanf(line, "%s %s %s %1s", first_word, second_word, third_word, extra);
        if (strlen(first_word) > 1 && first_word[strlen(first_word) - 1] == ':') {
            in_label = 1;
        }
        /* handle commands */
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
        /* If the command is entry */
        if (strcmp(current_command, ".entry") == 0) {
            error_type = processEntry(head, operands, line_number, &error_flag);
            if (error_type == 1)
                continue;
        }
        /* Else if the command is data, string or extern, they have already been dealt with in the first pass. */
        else if (strcmp(current_command, ".data") == 0 || strcmp(current_command, ".string") == 0 || strcmp(current_command, ".extern") == 0)
            continue;
        /* Else it's a command */
        else {
            operand_count = 0; /* reset the operand count */
            token = strtok(operands, ",");
            while (token != NULL && operand_count < 4) {
                while (*token != '\0' && isspace((unsigned char) *token)) /* Skip whitespace */
                    token++;
                /* Save the operands and get the address mode */
                parsed_operands[operand_count] = token;
                address_mode[operand_count] = getAddressMode(parsed_operands[operand_count]);
                operand_count++;
                token = strtok(NULL, ","); /* Go to the next operand */
            }

            for (index = 0; index < operand_count; index++) {
                if (address_mode[index] == 1 || address_mode[index] == 2) {
                    symbol_name = strtok(parsed_operands[index], " \n\t\r"); /* Get the symbol name without any whitespace */

                    if (address_mode[index] == 2)
                        symbol_name = parsed_operands[index] + 1; /* Symbol Name is after the percent sign */
                    
                    symbol = findSymbol(symbol_name, head);
                    if (symbol == NULL) {
                        fprintf(stderr, "ERROR at line %d: Symbol %s was not found.\n", line_number, symbol_name);
                        error_flag = 1;
                        continue;
                    }

                    
                    if (symbol->is_extern == 1) {
                        if (address_mode[index] == 2) {
                            fprintf(stderr, "ERROR at line %d: Cannot use external symbol with relative addressing.\n", line_number);
                            error_flag = 1;
                            continue;
                        }
                        /* If the first external symbol was found, create a .ext file. */
                        if (error_flag == 1)
                            continue;
                        if (ext_output == NULL) {
                            sprintf(new_ext_file_name, "%s.ext", file_name);

                            ext_output = fopen(new_ext_file_name, "w");
                            if (ext_output == NULL) {
                                fprintf(stderr, "ERROR: Error creating .ext file.\n");
                                remove(new_ext_file_name);
                                return 1;
                            }
                        }

                        offset = (ic - IC_INIT_VALUE) + 1 + index;
                        /* Save the symbol name and its ic value at the current moment */
                        fprintf(ext_output, "%s %04d\n", symbol->name, (offset + IC_INIT_VALUE));
                        code_image[offset] = 0;
                        are_image[offset] = 'E';
                    }
                    else if (address_mode[index] == 1) {
                        code_image[offset] = symbol->value;
                        are_image[offset] = 'R';
                    }

                    else if (address_mode[index] == 2) {
                        code_image[offset] = (symbol->value - (ic + 1 + index)) & MASK_12_BITS; /* We mask with 0xFFF so that negative numbers will not print more than 3 characters. */
                        are_image[offset] = 'A';
                    }
                } 
            }

            ic += (1 + operand_count);
        }
    }
    /* If we ever opened the file, close it. */
    if (ext_output != NULL) {
        fclose(ext_output);
        if (error_flag == 1)
            remove(new_ext_file_name);
    }
    /* Save the files if there's no error. */
    if (error_flag == 0) {
        export_ob = exportObFile(file_name, are_image, ic, dc, code_image, data_image);
        export_ent = exportEntFile(file_name, head);
    }

    freeSymbolList(head);
    /* If saving the file was unsuccessful, return 1 */
    if (export_ob == 1 || export_ent == 1)
        return 1;

    return error_flag; /* returns 0 if there's no error. */
}