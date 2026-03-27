#include "executor.h" 
#include <unistd.h> //fork, exec, dup2, pipe - the unix syscall wrappers
#include <sys/wait.h> //waitpid and status macros
#include <stdio.h> //perror, printf, fgets
#include <stdlib.h> //exit, malloc, free

// runs a simple command with no pipes or redirections
// forks a chil, replaces program, parent waits for child to finish
void execute_simple(char ** argv){

    if (argv == NULL || argv[0]  == NULL)
        return;

    pid_t pid = fork();
    if (pid < 0){
        perror("fork");
        return;
    }


    if (pid == 0){
        //child
        //create new program with execvp and check for failure

        execvp(argv[0], argv);

        // will only execute on failure since execvp doesn't return on success
        perror(argv[0]);

        exit(EXIT_FAILURE);

    }
    else{
        //parent
        int status;
        waitpid(pid, &status, 0);
    }
}