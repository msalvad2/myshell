
#ifndef SHELL_H
#define SHELL_H

#define MAX_INPUT 1024
#define MAX_ARGS 128
#define MAX_CMDS 16
#define PROMPT "myshell> "
#define SHELL_NAME "myshell"

#define ERR_NONE 0
#define ERR_TOO_MANY_ARGS 1
#define ERR_BAD_PIPE 2
typedef struct{
    char ** argv;
    char * infile;
    char * outfile;
    int append;
    int error; //1 = too many arguments
} cmd_t;

typedef struct{
    cmd_t cmds[MAX_CMDS];
    int num_cmds;
    // 1 - background job "&" | 0 - foreground
    int background; 
    int error; // 1 = too many arguments 2 = bad pipe syntax
    
} pipeline_t;

#endif