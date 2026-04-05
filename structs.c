#include "structs.h"

char *reserved_words[] = {"r1","r2","r3","r4","r5","r6","r7","mov","cmp","add","sub","lea","clr","not","inc","dec",
                        "jmp","bne","jsr","red","prn","rts","stop", ".data", ".string", ".entry", ".extern", "mcro", "mcroend"};

/* Command list with their opcodes, functs, number of words and valid modes for source and destination. */
Command command_table[16] = {
    {"mov",  0,  0, 2, 11, 10},
    {"cmp",  1,  0, 2, 11, 11},
    {"add",  2, 10, 2, 11, 10},
    {"sub",  2, 11, 2, 11, 10},
    {"lea",  4,  0, 2, 2, 10},
    {"clr",  5, 10, 1, 0, 10},
    {"not",  5, 11, 1, 0, 10},
    {"inc",  5, 12, 1, 0, 10},
    {"dec",  5, 13, 1, 0, 10},
    {"jmp",  9, 10, 1, 0, 6},
    {"bne",  9, 11, 1, 0, 6},
    {"jsr",  9, 12, 1, 0, 6},
    {"red", 12,  0, 1, 0, 10},
    {"prn", 13,  0, 1, 0, 11},
    {"rts", 14,  0, 0, 0, 0},
    {"stop", 15, 0, 0, 0, 0}
};