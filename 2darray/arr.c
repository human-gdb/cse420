#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct String{
	char* str;
}String;


#define BUFFERSIZE 10
#define STRINGSIZE 1024

int cur_put = 0;
int cur_get = 0;



void put(char *str,String *buf){
	buf[cur_put].str = strdup(str);
	printf("\n%s -> was placed into the buffer",buf[cur_put].str);
	cur_put++;
}

char *getitem(String *buf){
	if(cur_get >= cur_put) return NULL;
	cur_get++;
	return buf[cur_get-1].str;
}

int main(){

	String *buffer = calloc(10,sizeof(String));
	put("String1",buffer);
	put("String2",buffer);
	put("String3",buffer);
	put("String4",buffer);
	put("String5",buffer);
	
	printf("\n%s",getitem(buffer));
	printf("\n%s",getitem(buffer));
	printf("\n%s",getitem(buffer));
	printf("\n%s",getitem(buffer));
	printf("\n%s",getitem(buffer));
	

} 



