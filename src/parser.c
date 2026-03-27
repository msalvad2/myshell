#include "parser.h"
#include <string.h> //strtok, strcmp, strlen
#include <stdlib.h> //malloc, free, exit, atoi
#include <stdio.h> //perror, printf, fgets, I/O functions

#define MAX_ARGS 64

char ** parse_input(char * input){
    //starts at zero to check current placement then increments to update next spot available in list of strings
    int count = 0;

    if (input == NULL)
        return NULL;

    char ** argv = malloc(sizeof(char *) * MAX_ARGS);

    if (argv == NULL){
        perror("malloc");
        return NULL;
    }
    //first read to set strtok in input string
    argv[0] = strtok(input, " \t\n");

    //pick up where we left at in input and continue parsing
    while( argv[count] != NULL) {
        ++count;
        argv[count] = strtok(NULL, " \t\n");
    }

    return argv;
}