
#include <stdio.h>
#include "shell.h"
#include "builtins.h"
#include "parser.h"
#include "executor.h"
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "signals.h"
#include "jobs.h"

int main(void){
    char input[MAX_INPUT];
    
    // myshell should not be affected by signals such as
    // Ctrl+c, Ctrl+z, and background process writing to terminal
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, sigchld_handler);

    while(1){
        
        printf(PROMPT);
        fflush(stdout);
        print_done_jobs();
        if(fgets(input, sizeof(input), stdin) == NULL)
            break;
        // create a copy to send to execute if it is a background job
        // we don't pass pipeline.cmds because it doesn't have the "|"
        char cmd_original[MAX_INPUT];
        strncpy(cmd_original,input, MAX_INPUT - 1);
        cmd_original[MAX_INPUT - 1] = '\0';
        // if command has "&" we remove it
        cmd_original[strcspn(cmd_original, "&")] = '\0';
        cmd_original[strcspn(cmd_original, "\n")] = '\0'; // strips newline

        // remove the /n after user input
        input[strcspn(input, "\n")] = '\0';

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
                execute_simple(&pipeline, cmd_original);
            }
        }
        //pipes
        else{
            execute_pipeline(&pipeline, cmd_original);
        }
        
        for ( int i = 0; i < pipeline.num_cmds; ++i){
            free(pipeline.cmds[i].argv);
        }
 


    }


    return 0;
}