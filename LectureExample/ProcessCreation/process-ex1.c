#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(){

    pid_t x=0, y=0;
    x = fork();

    if(x == 0){
        y = fork();
    }

    printf("x = %ld\n",x);
    printf("y = %ld\n",y);
    exit(0);
    return 0;
}