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
        cmd.argv[count] = strtok_r(NULL, " \t\n", &saveptr);
        }
    }

    return cmd;
}

pipeline_t parse_pipeline(char * input){
    pipeline_t pipeline;
    pipeline.num_cmds = 0;
    pipeline.background = 0;

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
        return pipeline;
    }

    while (segment != NULL && pipeline.num_cmds < MAX_ARGS){
        pipeline.cmds[pipeline.num_cmds] = parse_input(segment);
        pipeline.num_cmds +=1;
        segment = strtok_r(NULL, "|", &saveptr);
    }

    return pipeline;

}