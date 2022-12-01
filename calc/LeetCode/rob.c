#include <stdio.h>
#include <stdlib.h>



int *prime(int N){
 int * arr = malloc(sizeof(int)*N);
 int p = 0;

 for(int i=10;i<N;i++){  //
 
   for(int j=2;j<i;j++){ // N/ all number less than N

     if(i%j==0) break;
     else if(j == i-1) { 
      if(i%j != 0){ arr[p]=i; p++;  printf(" %d ",i);}
     }

    }   
 }
return arr;
}





int main(){

    prime(15);
 return 0;
}