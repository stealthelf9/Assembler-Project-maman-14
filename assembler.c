#include "pre_assembler.h"

int main() {
    char filename[256];
    FILE *input = fopen("asdf.as", "r");
    preAssembler(input, "asdf.as");
    return 0;
}