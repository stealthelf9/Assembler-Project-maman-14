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