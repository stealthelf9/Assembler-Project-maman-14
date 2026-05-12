#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "pre_assembler.h"
#include "first_pass.h"
#include "second_pass.h"

/**
 * main
 * Processes the input files and gives them to the pre-assembler, then first pass and then second pass.
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @return 0 for success
 */
int main(int argc, char *argv[]);

#endif