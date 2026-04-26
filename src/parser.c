#include "parser.h"
#include "shell.h"
#include <string.h> //strtok_r, strcmp, strlen
#include <stdlib.h> //malloc, free, exit, atoi
#include <stdio.h> //perror, printf, fgets, I/O functions


cmd_t parse_input(char * input){
    //starts at zero to check current placement then increments to update next spot available in list of strings
    int count = 0;

    if (input == NULL){
        cmd_t empty = {0};
        return empty;
    }

    //initialize
    cmd_t cmd;
    cmd.argv = NULL;
    cmd.infile = NULL;
    cmd.outfile = NULL;
    cmd.append = 0;
    cmd.error = ERR_NONE;


    cmd.argv = malloc(sizeof(char *) * MAX_ARGS);


    if (cmd.argv == NULL){
        perror("malloc");
        cmd_t empty = {0};
        return empty;
    }
    //first read to set strtok_r in input string
    char *saveptr;
    cmd.argv[0] = strtok_r(input, " \t\n", &saveptr);

    //pick up where we left at in input and continue parsing
    while( cmd.argv[count] != NULL) {
        // we don't increment count in the if statements because
        // we replace >, <, >> with what comes next
        if (strcmp(cmd.argv[count], ">") == 0){
            // output truncate
            cmd.outfile = strtok_r(NULL, " \t\n", &saveptr);
            //overwrites ">" with the next string sinc.argv should not contain ">, <, >>"
            cmd.argv[count] = strtok_r(NULL, " \t\n", &saveptr);
    
        }
        else if (strcmp(cmd.argv[count], "<") == 0){
            // input
            cmd.infile = strtok_r(NULL, " \t\n", &saveptr);
            cmd.argv[count] = strtok_r(NULL, " \t\n", &saveptr);

        }
        else if (strcmp(cmd.argv[count], ">>") == 0){
            // outpu.append
            cmd.append = 1;
            cmd.outfile = strtok_r(NULL, " \t\n", &saveptr);
            cmd.argv[count] = strtok_r(NULL, " \t\n", &saveptr);
        }
       else{ 
        
            ++count;
            //fprintf(stderr, "DEBUG count=%d MAX_ARGS-1=%d\n", count, MAX_ARGS-1);
            // Commands exceed the MAX Arguments
            if (count > MAX_ARGS -1) {
                // Own error not system error so we use fprintf(stderr)
                fprintf(stderr, "myshell: too many arguments\n");
                cmd.argv[0] = NULL;
                cmd.error = ERR_TOO_MANY_ARGS;
                break;
            }
            
        cmd.argv[count] = strtok_r(NULL, " \t\n", &saveptr);


        }
    }

    return cmd;
}

pipeline_t parse_pipeline(char * input){
    pipeline_t pipeline;
    pipeline.num_cmds = 0;
    pipeline.background = 0;
    pipeline.error = 0;
    

    if (!input) return pipeline;

    //checks to see if input has &
    char *background = strchr(input, '&');
    
    //background job
    if (background){
        *background = '\0';
        pipeline.background = 1;
    }
    // holds the segment of input
    char * segment;
    char * saveptr;



    segment = strtok_r(input, "|", &saveptr);

    if (!segment) {
        pipeline.error = ERR_BAD_PIPE;
        pipeline.num_cmds = 0;
        return pipeline;
    }

    while (segment != NULL && pipeline.num_cmds < MAX_ARGS){
        pipeline.cmds[pipeline.num_cmds] = parse_input(segment);
        //checks if piped commands are invalid: e.g ls |  | grep .c
        if (pipeline.cmds[pipeline.num_cmds].argv[0] == NULL){
            if (pipeline.cmds[pipeline.num_cmds].error == ERR_TOO_MANY_ARGS) {
                pipeline.error = ERR_TOO_MANY_ARGS;
            }
            else{
                pipeline.error = ERR_BAD_PIPE;
            }
            // printf("bash: syntax error near unexpected token `|'\n");
            // free all created commands to avoid memory leak
            for (int i = 0; i <= pipeline.num_cmds; ++i){
            free(pipeline.cmds[i].argv);
            }
            pipeline.num_cmds = 0; 
            return pipeline;
        }

        pipeline.num_cmds +=1;
        segment = strtok_r(NULL, "|", &saveptr);
        
    }
    return pipeline;

}
