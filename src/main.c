
#include <stdio.h>
#include "shell.h"
#include "builtins.h"
#include "parser.h"
#include "executor.h"
#include <string.h>
#include <stdlib.h>

int main(void){
    char input[MAX_INPUT];
    


    while(1){
        
        printf(PROMPT);

        if(fgets(input, sizeof(input), stdin) == NULL)
            break;
        // remove the /n after user input
        //input[strcspn(input, "\n")] = '\0';
        cmd_t* cmd = parse_input(input);
        if (cmd == NULL) continue;

        // if user entered space, enter, or tab only
        if (cmd->argv == NULL || cmd->argv[0] == NULL){
            free(cmd->argv);
            free(cmd);
            continue;
        }

        //changes current process state
        if(is_builtin(cmd->argv[0]))
            run_builtin(cmd->argv);
        //will create a child process to complete task
        else{
            execute_simple(cmd);
        }

        free(cmd->argv);
        free(cmd);
    }


    return 0;
}