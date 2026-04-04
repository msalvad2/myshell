#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "shell.h"

void execute_simple(cmd_t* cmd);
void execute_pipeline(pipeline_t* pipeline);
#endif