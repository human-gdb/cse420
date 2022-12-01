#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int pagefault;
int framesize;
typedef struct frame frame;
typedef struct que que;
void addtail(que *list, int val);
void initFIFO(que *myq);
frame *search(que *q, int sval);
void addpageFIFO(que *q, int val);


typedef struct frame{
	frame *prev;
	int page;
	frame *next;
}frame;

typedef struct que{
	frame *head;
	int size;
	frame *tail;
}que;

void printarr(int *arr){
	for(int *p = arr; *p != '\0'; p++)
	printf(" %d ",*p);
	printf("\n");
}

int* tokenize(char *refstr){

	int arrLen = 0;
	for(char *s = refstr; *s != '\0' ; s++)
		arrLen = (*s == ',' ) ? arrLen+1 : arrLen;
	arrLen++;
	printf("%d size allocated for ref array\n",arrLen );
	int *refArr = calloc(arrLen,sizeof(int));

		char buffer[1024];
		strcpy(buffer,refstr);
	 	char *word,*ptr=NULL;
	 	char delim[] =  ",";
	 	int i = 0; 
	for(word=strtok_r(buffer,delim,&ptr);word != NULL;word=strtok_r(NULL,delim,&ptr) ) {
                  word = strdup(word);
	          //printf("%d\n",atoi(word));	
         	  refArr[i] = atoi(word);i++;
		   free(word);
        }
              
          
	return refArr;
}

void FIFO(char *refstr, int frameSize){
	
	int *refarr = tokenize(refstr);
	que *FIFOque = NULL;
	initFIFO(FIFOque);
	
	//int frame[frameSize];
	
	//for(int *f = frame; *f != '\0' ;f++)*f = -1;
	//printarr(frame);
	printarr(refarr);
	
	for(int *j = refarr; *j != '\0' ; j++){
		addpageFIFO(FIFOque,*j);
	}
	printf("pf: %d", pagefault);
}

void initFIFO(que *myq){
	myq = malloc(sizeof(que));
	myq->head = NULL;
	myq->tail = NULL;	
}

void addtail(que *list, int val){
	frame* node = calloc(1,sizeof(frame));
	node->page = val;
	if (list->head == NULL && list->tail == NULL) {
	    list->head = node;
	    list->tail = node;
	} else {
	    list->tail->next = node;
	    node->prev = list->tail;
	    list->tail = node;
	}
	list->size++;
}

void addhead(que *list, int val){
	frame* node = calloc(1,sizeof(frame));
	node->page = val;
	if (list->head == NULL && list->tail == NULL) {
	    list->head = node;
	    list->tail = node;
	} else {
	    list->head->prev = node;
	    node->prev = NULL;
	    node->next = list->head;
	    list->head = node;
	}
	list->size++;
}

int removetail(que *list){
	frame *t = list->tail;
	list->tail = list->tail->prev;
	list->tail->next = NULL;
	int v = t->page;
	free(t); 
	list->size--;
	return v;
}

void addpageFIFO(que *q, int val){
	if(search(q,val) == NULL){
		pagefault++;
		//addhead(q,val);
		//if(q->size > framesize) removetail(q);
	}
	else
		return;

}

frame *search(que *q, int sval){
	frame *n = q->head;
	//while(n != NULL){
	
	//if(n->page == sval) return n;
	//n = n->next;
	//}
	return NULL;
}

int main(int argc, char* argv[]){
	
	FIFO(argv[1],4);
	
	return 0;
}
