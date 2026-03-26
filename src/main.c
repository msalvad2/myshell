
#include <stdio.h>

int main(void){
    char input[1024];

    while(1){
        
        printf("myshell> ");

        if(fgets(input, sizeof(input), stdin) == NULL)
            break;

        printf("Your input: %s", input);
        printf("\n");

    }

    printf("Thank you for participating");

    return 0;
}