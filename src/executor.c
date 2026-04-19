#include "executor.h"
#include "shell.h"
#include <unistd.h> //fork, exec, dup2, pipe - the unix syscall wrappers
#include <sys/wait.h> //waitpid and status macros
#include <stdio.h> //perror, printf, fgets
#include <stdlib.h> //exit, malloc, free
#include <fcntl.h> // open, O_WRONLY, O_CREAT ...
#include "jobs.h"

// runs a simple command with no pipes or redirections
// forks a chil, replaces program, parent waits for child to finish
void execute_simple(pipeline_t* pipeline, char* cmd_original){

    if (pipeline->cmds[0].argv == NULL || pipeline->cmds[0].argv[0]  == NULL)
        return;

    pid_t pid = fork();
    
    if (pid < 0){
        perror("fork");
        return;
    }


    if (pid == 0){
        //child
        //create new program with execvp and check for failure

        //changes child group id 
        setpgid(0,0);
        //restore signals default behavior
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        if ( pipeline->cmds[0].infile){
            int fd;

            fd = open(pipeline->cmds[0].infile, O_RDONLY, 0);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            //0 not points to input.txt
            dup2(fd, STDIN_FILENO);
            close(fd);

        }
        if( pipeline->cmds[0].outfile){
            int fd;

            if (pipeline->cmds[0].append == 0){
            fd = open(pipeline->cmds[0].outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0){
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            }
            
            else{
                fd = open(pipeline->cmds[0].outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd < 0){
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
        }


        execvp(pipeline->cmds[0].argv[0], pipeline->cmds[0].argv);

        // will only execute on failure since execvp doesn't return on success
        perror(pipeline->cmds[0].argv[0]);

        exit(EXIT_FAILURE);

    }
    else{
        //parent
        int status;
        // changing childs group id to avoid RACE condition
        setpgid(pid,pid);

        if (pipeline->background == 1) {
            int job_id = 0;
            job_id = add_job(pid, cmd_original);
            printf("[%d]    %d\n", job_id, pid);
            return;
        }
        // Hands child terminal so it will recieve signals.
        tcsetpgrp(STDIN_FILENO, pid);
        
        waitpid(pid, &status, 0);

        // myshell gets terminal back
        tcsetpgrp(STDIN_FILENO, getpgrp());
    }
}

void execute_pipeline(pipeline_t *pipeline, char *cmd_original){
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
        
        // children will not make it past this if statement
        // they are ended either by exec or exit()
        if (pid[i] == 0){

        // All children should have the same group id as the first child
        // so that they all are affected by same signals
        if (i == 0){
            setpgid(0,0);
        }
        else{
        setpgid(0, pid[0]);
        }
        
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
          
           
            if (i > 0){
                //wire stdin (subtract 1 to read from previous pipe)
                dup2(pipes[i -1][0], STDIN_FILENO);

            }
            if (i < pipeline->num_cmds - 1){
                //wire stdou
                dup2(pipes[i][1], STDOUT_FILENO);
            }
              // if using redirection (<)
            if (pipeline->cmds[i].infile){
                int fd = 0;
                fd = open(pipeline->cmds[i].infile, O_RDONLY, 0);
                if (fd < 0){
                    perror("open ");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
             // if using redirection ( > and >>)
           if( pipeline->cmds[i].outfile){
            int fd;

            if (pipeline->cmds[i].append == 0){
            fd = open(pipeline->cmds[i].outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0){
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            }
            
            else{
                fd = open(pipeline->cmds[i].outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd < 0){
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
        }

            
            // After wiring stdin & stdout if necessary we must close all
            // pipe ends before exec
            for (int j = 0; j < pipeline->num_cmds - 1; ++j){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            //Execute exec
            //After exec the children is gone
            execvp(pipeline->cmds[i].argv[0], pipeline->cmds[i].argv);

            //only execute if execvp fails
            perror(pipeline->cmds[i].argv[0]);
            exit(EXIT_FAILURE);
        }
            // Avoids race condition
            setpgid(pid[i], pid[0]);
        }
    //Close parents pipe ends
    for ( int i = 0; i < pipeline->num_cmds - 1; ++i){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    if (pipeline->background == 1) {
        int job_id = 0;
        job_id = add_job(pid[0], cmd_original);
        printf("[%d]    %d\n",job_id, pid[0]);
        return;
    }
    //give child terminal control for signals
    tcsetpgrp(STDIN_FILENO, pid[0]);


    // Wait for children to avoid Zombie Processes
    for (int i = 0; i < pipeline->num_cmds; ++i){
        int status;
        waitpid(pid[i], &status, 0);
    }
    // Hands terminal control back to myshell
    tcsetpgrp(STDIN_FILENO, getpgrp());

    }