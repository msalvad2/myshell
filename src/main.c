
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

        pipeline_t pipeline = parse_pipeline(input);

        // if user entered space, enter, or tab only
        if (pipeline.cmds[0].argv == NULL || pipeline.cmds[0].argv[0] == NULL){
            free(pipeline.cmds[0].argv);
            continue;
        }

        if( pipeline.num_cmds == 1){
            //will change current process state
            if (is_builtin(pipeline.cmds[0].argv[0])){
                run_builtin(pipeline.cmds[0].argv);
            }
            //No pipes
            else{
                execute_simple(&pipeline.cmds[0]);
            }
        }
        //pipes
        else{
            execute_pipeline(&pipeline);
        }
        
        for ( int i = 0; i < pipeline.num_cmds; ++i){
            free(pipeline.cmds[i].argv);
        }
 


    }


    return 0;
}