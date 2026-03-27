#include <stdio.h> // perror,printf
#include <stdlib.h> //exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //strcmp, strncmp
#include <unistd.h> // chdir (change directory)
#include "shell.h" 
#include "builtins.h"

int is_builtin(char * input){
    if(strcmp(input, "cd") == 0 || strcmp(input, "exit") == 0)
        return 1;
    else
        return 0;
}

int run_builtin(char **input){
   if (strcmp(input[0], "exit") == 0)
        exit(EXIT_SUCCESS);

   else if(strcmp(input[0], "cd") == 0){
         int status;
         // triggers if user only typed cd with no argument
         // takes user home
         if (input[1] == NULL){
            chdir(getenv("HOME"));
            return 0;
         }

        status = chdir(input [1]);

        if(status == 0 )
            return 0;
        else{
            perror("cd");
            return 1;
        }
            
   }
   else{
    printf("Error has occured");
    exit(EXIT_FAILURE);

   }

   return 0; // unreachable but silences compiler warning

}