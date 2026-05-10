#include "assembler.h"

int main(int argc, char *argv[]) {
    char *file_name;
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
    SymbolNode *head = NULL;

    FILE *input;

    for (index = 1; index < argc; index++) {
        /* Reset everything for the next file. */
        file_name = argv[index];
        ic = 100;
        dc = 0;
        head = NULL;

        sprintf(new_file_name, "%s.as", file_name);
        input = fopen(new_file_name, "r");
        if (input == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s.as file.\n", file_name);
            continue;
        }

        pre_assembler_val = preAssembler(input, file_name);
        fclose(input);

        if (pre_assembler_val == 1)
            continue;
        
        sprintf(new_file_name, "%s.am", file_name);
        input = fopen(new_file_name, "r");
        if (input == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s.am file.\n", file_name);
            continue;
        }

        first_pass_val = firstPass(input, file_name, are_image, code_image, data_image, &ic, &dc, &head);

        if (first_pass_val == 1) {
            fclose(input);
            continue;
        }

        rewind(input);

        second_pass_val = secondPass(input, file_name, are_image, code_image, data_image, dc, head);
        fclose(input);
    }
    return 0;
}