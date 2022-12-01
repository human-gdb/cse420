#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int count = 0;
int main(){

pid_t x = 0 , y = 0 , z = 0;
    x = fork();
    int i = 5;

    if(x==0){
        y = fork();
    } 

    z = fork();

    count++;printf("x = %d \t%d\n",x,count);
    count++;printf("y = %d \t%d\n",y,count);
    count++;printf("z = %d \t%d\n",z,count);

}