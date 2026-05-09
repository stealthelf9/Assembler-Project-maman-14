#include "assembler.h"

int main(int argc, char *argv[]) {
    char file_name[MAX_FILE_NAME_LENGTH];
    char new_file_name[MAX_FILE_NAME_LENGTH];

    int index = 1;
    /* Variables for the validity of the phases of the assembler. */
    int pre_assembler_val = 1;
    int first_pass_val = 1;
    int second_pass_val = 1;

    int ic = 100;
    int dc = 0;
    unsigned int code_image[4096] = {0};
    unsigned int data_image[4096] = {0};
    char are_image[4096] = "";
    SymbolNode *head = NULL

    FILE *input;

    for (index = 1; index < argc; index++) {

        sprintf(new_file_name, "%s.as", file_name);
        input = fopen(argv[index], "r");
        if (input == NULL) {
            fprintf(stderr, "ERROR: Error creating .as file.\n");
            remove(new_file_name);
            return 1;
        }

        pre_assembler_val = preAssembler(input, new_file_name);
        if (pre_assembler_val == 1)
            continue;
        
        first_pass_val = firstPass();
    }
    return 0;
}