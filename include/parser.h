#ifndef PARSER_H
#define PARSER_H

#include "shell.h"

cmd_t parse_input(char* input);
pipeline_t parse_pipeline(char * input);
char *next_token(char* str, const char* delim, char ** saveptr);
void strip_quotes(char * token);

#endif
