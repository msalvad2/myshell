#ifndef JOBS_H

#define JOBS_H

#include <unistd.h>  // pid_t

#define JOB_RUNNING 1
#define JOB_DONE 2
#define MAX_CMD 256
#define MAX_JOB 64

int add_job(pid_t, char *cmd);
void remove_job(pid_t pid);
void mark_job_done(pid_t pid);
void print_done_jobs(void);
void print_all_jobs(void);
#endif