#include "jobs.h"
#include "string.h"
#include "stdio.h"
typedef struct{
    pid_t pid;
    int job_id;
    char cmd[MAX_CMD];
    // 1 = running | 2 = done
    int state; 
} job_t;
static job_t jobs[MAX_JOB];

void mark_job_done(pid_t pid){
    for ( int i = 0; i < MAX_JOB; ++i){
        if (jobs[i].pid == pid){
            jobs[i].state = JOB_DONE;
        }
    }
}
int add_job(pid_t pid, char* cmd){

    for (int i = 0; i < MAX_JOB; ++i){
        // empty job
        if (jobs[i].pid == 0){
            jobs[i].pid = pid;
            jobs[i].job_id = i + 1;
            // use strncpy because it copies fixed size buffer safely
            strncpy(jobs[i].cmd, cmd, MAX_CMD - 1);
            jobs[i].cmd[MAX_CMD - 1] = '\0';
            jobs[i].state = JOB_RUNNING; //running
            return jobs[i].job_id;           
        }
    }
    printf("No More Jobs Allowed\n");
    return -1;
}
// resets the job[i] to zero to be used again later on
void remove_job(pid_t pid){
    for (int i = 0; i < MAX_JOB; ++i){
        if (jobs[i].pid == pid){
            memset(&jobs[i], 0, sizeof(job_t));
        }
    }
}
// prints jobs that finished at the top of REPL loops then removes them
void print_done_jobs(void){
    for (int i = 0; i < MAX_JOB; ++i){
        //if job exists
        if (jobs[i].pid != 0){
            // if job is done
            if (jobs[i].state == JOB_DONE){
                printf("[%d] DONE   %s\n", jobs[i].job_id, jobs[i].cmd);
                memset(&jobs[i], 0, sizeof(job_t));
            }

        }
    }
}
// Called when the user enters jobs (used as builtin function because it doesn't change terminal state)
void print_all_jobs(void){
    for (int i = 0; i < MAX_JOB; ++i){
        if (jobs[i].pid != 0){
            if (jobs[i].state == JOB_RUNNING ){
                printf("[%d] RUNNING    %s\n", jobs[i].job_id, jobs[i].cmd);
            }
            else{
                printf("[%d] DONE   %s\n", jobs[i].job_id, jobs[i].cmd);
            }
        }
    }
}