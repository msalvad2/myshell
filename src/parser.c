#include "parser.h"
#include "shell.h"
#include <string.h> //strtok, strcmp, strlen
#include <stdlib.h> //malloc, free, exit, atoi
#include <stdio.h> //perror, printf, fgets, I/O functions


cmd_t* parse_input(char * input){
    //starts at zero to check current placement then increments to update next spot available in list of strings
    int count = 0;

    if (input == NULL)
    return NULL;

    //initialize
    cmd_t* cmd = malloc(sizeof(cmd_t));
    cmd->argv = NULL;
    cmd->infile = NULL;
    cmd->outfile = NULL;
    cmd->append = 0;


    cmd->argv = malloc(sizeof(char *) * MAX_ARGS);

    if (cmd->argv == NULL){
        perror("malloc");
        return NULL;
    }
    //first read to set strtok in input string
    cmd->argv[0] = strtok(input, " \t\n");

    //pick up where we left at in input and continue parsing
    while( cmd->argv[count] != NULL) {
        
        if (strcmp(cmd->argv[count], ">") == 0){
            // output truncate
            cmd->outfile = strtok(NULL, " \t\n");
            //overwrites ">" with the next string since argv should not contain ">, <, >>"
            cmd->argv[count] = strtok(NULL, " \t\n");
    
        }
        else if (strcmp(cmd->argv[count], "<") == 0){
            // input
            cmd->infile = strtok(NULL, " \t\n");
            cmd->argv[count] = strtok(NULL, " \t\n");

        }
        else if (strcmp(cmd->argv[count], ">>") == 0){
            // output append
            cmd->append = 1;
            cmd->outfile = strtok(NULL, " \t\n");
            cmd->argv[count] = strtok(NULL, " \t\n");
        }
        else{
            ++count;
        cmd->argv[count] = strtok(NULL, " \t\n");
        }
    }

    return cmd;
}