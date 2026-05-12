#include "utils.h"
/**
 * findCommand
 * Finds the command int the command table based on the command name.
 * @param command_name The command name you want to find.
 * @return -1 if the command wasn't found or the index of the command table if the command was found.
 */
int findCommand(char *command_name) {
    int index = 0;
    
    while (index < ARRAY_SIZE(command_table)) {
        if (strcmp(command_name, command_table[index].name) == 0)   
            return index;
        
        index++;
    }
    return -1;
}

/**
 * getAddressMode
 * Gets the address mode of the operand.
 * @param operand The operand.
 * @return The address mode of the operand.
 */
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