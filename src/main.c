
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
#include "readline/history.h" //add_history()
#include "readline/readline.h" //readline()
#include "string.h"

int main(void){
      // char input[MAX_INPUT]; replaced by readline
    
    // myshell should not be affected by signals such as
    // Ctrl+c, Ctrl+z, and background process writing to terminal
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, sigchld_handler);

    while(1){
        print_done_jobs();
        char* input = readline("myshell>> ");

        if (!input){
            break;
        }
        char empty[] = " \t\n";
        size_t length = strlen(input);
        if (length == strspn(input, empty)) {
            free(input);
            continue;
        }

        add_history(input);

        // create a copy to send to execute if it is a background job
        // we don't pass pipeline.cmds because it doesn't have the "|"
        char cmd_original[MAX_INPUT];
        strncpy(cmd_original,input, MAX_INPUT - 1);
        cmd_original[MAX_INPUT - 1] = '\0';
        // if command has "&" we remove it
        cmd_original[strcspn(cmd_original, "&")] = '\0';
        cmd_original[strcspn(cmd_original, "\n")] = '\0'; // strips newline
    

        pipeline_t pipeline = parse_pipeline(input);
        // empty input, too many command arguments, bad pipe syntax
        if (pipeline.num_cmds == 0){
            
            if (pipeline.error == ERR_BAD_PIPE){
                 fprintf(stderr, "bash: syntax error near unexpected token '|'\n");
            }
            
            free(input);
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

        free(input);
    }


    return 0;
}