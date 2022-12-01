#include <stdio.h>
#include <stdlib.h>
#define nl printf("\n")
void pushArr(int *arr,int l);

void printarr(int * arr,int l){
      nl;
    for(int i = 0;i<l;i++)
        printf("%d ",arr[i]);
    nl;
}

void rotateIt(int* nums, int numsSize, int k){
        k = numsSize%k;
        int *temp = malloc(numsSize*sizeof(nums[0]));
        int l = numsSize;
        int j =0;
        for(int i = 0; i<l ; i++){
            if(i+k>=l)
                temp[j++] = nums[i];
            else
            temp[i+k]=nums[i];
        }
        for(int i=0;i<l;i++) nums[i] = temp[i]; 
        printarr(nums,l);
    free(temp);
}


void rotate(int* nums, int numsSize, int k){
    
        pushArr(nums,numsSize);
    printarr(nums,numsSize);
}

void pushArr(int *arr,int l){
    int last = arr[l-1];
    int temp;
    for(int i=l-1;i>0;i--)
        arr[i] = arr[i-1];

        
    
    arr[0] = last;
}


int main(){
    int ar[] = {1,2,3};

    rotate(ar,3,2);

    return 0;
}