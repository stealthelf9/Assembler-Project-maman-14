/* This file contains all of the global definitions and structures used for the assembler.
 * The macro and symbol nodes all use a linked list.
 */

#ifndef STRUCTS_H
#define STRUCTS_H

#define MACRO_NAME_MAX_LENGTH 33 /* 32 for macro name + padding and then 1 buffer to check if the macro name length is longer than 31 characters */
#define SYMBOL_NAME_MAX_LENGTH 33 /* 32 for symbol name + padding and then 1 buffer to check if the symbol name length is longer than 31 characters */
#define MAX_LINE_LENGTH 82 /* 80 for the line, + padding and then 1 buffer to check if the line is longer than 81 characters */
#define MAX_FILE_NAME_LENGTH 256 /* Modern operating system limit. */
#define RESERVED_WORD_COUNT 26 /* We do not need .data, .string, .entry or for .extern because when checking for the symbol or macro name, they can't start with a dot.*/
#define MAX_MEMORY_SIZE 4096 /* Memory size of the imaginary computer. */
#define ARRAY_SIZE(array) ((sizeof(array)) / (sizeof(array[0])))
#define IC_INIT_VALUE 100
#define MASK_12_BITS 0xFFF

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

extern const Command command_table[16];
extern const char *reserved_words[RESERVED_WORD_COUNT];

#endif