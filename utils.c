#include "utils.h"

int findCommand(char *command_name) {
    int index = 0;
    
    while (index < ARRAY_SIZE(command_table)) {
        if (strcmp(command_name, command_table[index].name) == 0)   
            return index;
        
        index++;
    }
    return -1;
}

int getAddressMode(char *operand) {
    if (operand[0] == '#')
        return 0;
    else if (operand[0] == '%')
        return 2;
    else if ((operand[0] == 'r' && (operand[1] >= '0' && operand[1] <= '7')) && (isspace(operand[2]) || operand[2] == '\0'))
        return 3;
    else
        return 1;
}