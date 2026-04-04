
#ifndef SHELL_H
#define SHELL_H

#define MAX_INPUT 1024
#define MAX_ARGS 128
#define MAX_CMDS 16
#define PROMPT "myshell> "
#define SHELL_NAME "myshell"

typedef struct{
    char ** argv;
    char * infile;
    char * outfile;
    int append;
} cmd_t;

typedef struct{
    cmd_t cmds[MAX_CMDS];
    int num_cmds;
} pipeline_t;

#endif