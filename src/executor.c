#include "executor.h" 
#include <unistd.h> //fork, exec, dup2, pipe - the unix syscall wrappers
#include <sys/wait.h> //waitpid and status macros
#include <stdio.h> //perror, printf, fgets
#include <stdlib.h> //exit, malloc, free
#include <fcntl.h> // open, O_WRONLY, O_CREAT ...

// runs a simple command with no pipes or redirections
// forks a chil, replaces program, parent waits for child to finish
void execute_simple(cmd_t* cmd){

    if ( cmd == NULL) return;

    if (cmd->argv == NULL || cmd->argv[0]  == NULL)
        return;

    pid_t pid = fork();
    
    if (pid < 0){
        perror("fork");
        return;
    }


    if (pid == 0){
        //child
        //create new program with execvp and check for failure

        if ( cmd->infile){
            int fd;

            fd = open(cmd->infile, O_RDONLY, 0);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            //0 not points to input.txt
            dup2(fd, STDIN_FILENO);
            close(fd);

        }
        if( cmd->outfile){
            int fd;

            if (cmd->append == 0){
            fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0){
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            }
            
            else{
                fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd < 0){
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
        }


        execvp(cmd->argv[0], cmd->argv);

        // will only execute on failure since execvp doesn't return on success
        perror(cmd->argv[0]);

        exit(EXIT_FAILURE);

    }
    else{
        //parent
        int status;
        waitpid(pid, &status, 0);
    }
}