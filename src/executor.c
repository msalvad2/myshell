#include "executor.h"
#include "shell.h"
#include <unistd.h> //fork, exec, dup2, pipe - the unix syscall wrappers
#include <sys/wait.h> //waitpid and status macros
#include <stdio.h> //perror, printf, fgets
#include <stdlib.h> //exit, malloc, free
#include <fcntl.h> // open, O_WRONLY, O_CREAT ...

// runs a simple command with no pipes or redirections
// forks a chil, replaces program, parent waits for child to finish
void execute_simple(cmd_t* cmd){

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

void execute_pipeline(pipeline_t *pipeline){
    // we subract one because we always have one less pipe for the 
    // number of commands we have
    int pipes[MAX_CMDS - 1][2];
    for (int i = 0; i < pipeline->num_cmds - 1; ++i){
        pipe(pipes[i]);
    }

    // fork all children
    pid_t pid[MAX_CMDS];
    for (int i = 0; i < pipeline->num_cmds; ++i){
        pid[i] = fork();

        if (pid[i] == 0){
            if (i > 0){
                //wire stdin (subtract 1 to read from previous pipe)
                dup2(pipes[i -1][0], STDIN_FILENO);

            }
            if (i < pipeline->num_cmds - 1){
                //wire stdou
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            
            // After wiring stdin & stdout if necessary we must close all
            // pipe ends before exec
            for (int j = 0; j < pipeline->num_cmds - 1; ++j){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            //Execute exec
            execvp(pipeline->cmds[i].argv[0], pipeline->cmds[i].argv);

            //only execute if execvp fails
            perror(pipeline->cmds[i].argv[0]);
            exit(EXIT_FAILURE);
        }
        
        }

    //Close parents pipe ends
    for ( int i = 0; i < pipeline->num_cmds - 1; ++i){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    // Wait for children to avoid Zombie Processes
    for (int i = 0; i < pipeline->num_cmds; ++i){
        int status;
        waitpid(pid[i], &status, 0);

    }
    }

