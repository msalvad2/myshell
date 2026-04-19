#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "shell.h"

void execute_simple(pipeline_t* pipeline, char* cmd_original);
void execute_pipeline(pipeline_t* pipeline, char* cmd_original);
#endif