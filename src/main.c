
#include <stdio.h>
#include "shell.h"
#include "builtins.h"
#include "parser.h"
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
        char ** argv = parse_input(input);

        // if user entered space, enter, or tab only
        if (argv == NULL || argv[0] == NULL){
            free(argv);
            continue;
        }

        //changes current process state
        if(is_builtin(argv[0]))
            run_builtin(argv);
        //will create a child process to complete task
        else{
            execute_simple(argv);
        }

        free(argv);
    }


    return 0;
}