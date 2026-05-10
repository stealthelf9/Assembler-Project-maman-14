#ifndef STRUCTS_H
#define STRUCTS_H

#define MACRO_NAME_MAX_LENGTH 33 /* 32 for macro name + padding and then 1 buffer to check if the macro name length is longer than 31 */
#define SYMBOL_NAME_MAX_LENGTH 33 /* 32 for symbol name + padding and then 1 buffer to check if the symbol name length is longer than 31 */
#define MAX_LINE_LENGTH 82
#define MAX_FILE_NAME_LENGTH 256
#define RESERVED_WORD_COUNT 29
#define ARRAY_SIZE(array) ((sizeof(array)) / (sizeof(array[0])))

typedef struct MacroNode {
    char *name;
    char **code;
    int line_count;
    struct MacroNode *next;
} MacroNode;

typedef struct SymbolNode {
    char *name;
    int value;
    int is_code;
    int is_data;
    int is_entry;
    int is_extern;
    struct SymbolNode *next;
} SymbolNode;

typedef struct {
    char *name;
    unsigned int opcode;
    unsigned int funct;
    int operand_count; /* 0, 1, or 2 */
    unsigned int source_modes;
    unsigned int dest_modes;
} Command;

extern Command command_table[16];
extern char *reserved_words[RESERVED_WORD_COUNT];

#endif