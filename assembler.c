#include "assembler.h"

/**
 * main
 * Processes the input files and gives them to the pre-assembler, then first pass and then second pass.
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @return 0 for success
 */
int main(int argc, char *argv[]) {
    char *file_name;
    char new_file_name[MAX_FILE_NAME_LENGTH];

    int index = 1;
    /* Variables for the validity of the phases of the assembler. */
    int pre_assembler_val = 1;
    int first_pass_val = 1;

    int ic = IC_INIT_VALUE;
    int dc = 0;
    unsigned int code_image[MAX_MEMORY_SIZE] = {0};
    unsigned int data_image[MAX_MEMORY_SIZE] = {0};
    char are_image[MAX_MEMORY_SIZE] = "";
    SymbolNode *head = NULL;

    FILE *input;

    if (argc < 2) {
        fprintf(stderr, "ERROR: No input files provided.\nUsage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    for (index = 1; index < argc; index++) {
        /* Reset everything for the next file. */
        file_name = argv[index];
        ic = IC_INIT_VALUE;
        dc = 0;
        head = NULL;

        fprintf(stdout, "%s\n", file_name);
        /* Append .as to the file name */
        sprintf(new_file_name, "%s.as", file_name);
        input = fopen(new_file_name, "r");
        /* If it failed to to the next file */
        if (input == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s.as file.\n", file_name);
            continue;
        }

        pre_assembler_val = preAssembler(input, file_name);
        fclose(input);

        /* If it failed to to the next file */
        if (pre_assembler_val == 1)
            continue;

        /* Append .am to the file name */
        sprintf(new_file_name, "%s.am", file_name);
        input = fopen(new_file_name, "r");
        if (input == NULL) {
            fprintf(stderr, "ERROR: Cannot open %s.am file.\n", file_name);
            continue;
        }

        first_pass_val = firstPass(input, file_name, are_image, code_image, data_image, &ic, &dc, &head);

        /* If it failed to to the next file */
        if (first_pass_val == 1) {
            fclose(input);
            continue;
        }
        /* Go back to reading the beginning of the file */
        rewind(input);

        secondPass(input, file_name, are_image, code_image, data_image, dc, head);
        fclose(input);
    }
    return 0;
}