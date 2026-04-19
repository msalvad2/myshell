#include "signals.h"
#include <sys/wait.h> //waitpid and status macros
#include "jobs.h"

void sigchld_handler(int signal){
    (void)signal;
    int status;
    pid_t pid;

    while ( (pid = waitpid(-1, &status, WNOHANG)) > 0){
        mark_job_done(pid);
    }
    
}