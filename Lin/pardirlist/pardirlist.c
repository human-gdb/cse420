#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

char * outputfilename;
char *keyword;
int ParMode = 0;
typedef struct Node_TID Node_TID;
typedef struct LL LL;
LL *TID_list;
int testRun = 0;



int keyword_search(char * filePath,char *keyword);
void *makeThread(void* arg);
void XOR_Swap(int* a, int* b);


//create node structure
struct Node {
  char *path;
  int level;
  int i;
  int search;
  struct Node *next;
  struct Node *prev;
};

//create list structure
struct List {
  struct Node *head;
  struct Node *tail;
};


//storing thread ids
struct Node_TID {
    pthread_t tid;
    Node_TID *next;
};

struct LL{
    Node_TID* head;
    Node_TID* tail;
    int size;
};

//linked list for thread ids
struct LL* init_LinkedList(LL *list){
    list = malloc(sizeof(struct LL));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;

}

//func to appped thread id to the linked list
void addTID(LL* list,pthread_t tid){

    struct Node_TID* newTID = malloc(sizeof(struct Node_TID));
    newTID->tid = tid;
    newTID->next = NULL;

    if(list->head == NULL){
        list->head = newTID;
        list->head->next = NULL;
    }
    else{

        newTID->next= list->head;
        list->head = newTID;
    }
    list->size++;
}


//create Node with path and id
struct Node *create_node(char *path, int level, int i) {
  struct Node *node = malloc(sizeof(struct Node));
  if (node == NULL) {
    fprintf (stderr, "%s: Couldn't create memory for the node; %s\n", "linkedlist", strerror(errno));
    exit(-1);
  }
  node-> i = i;
  node->path = strdup(path);
  node->level = level;
  node->next = NULL;
  node->prev = NULL;
  node->search = -69;
  return node;
}

struct List *create_list() {
  struct List *list = malloc(sizeof(struct List));
  if (list == NULL) {
    fprintf (stderr, "%s: Couldn't create memory for the list; %s\n", "linkedlist", strerror (errno));
    exit(-1);
  }
  list->head = NULL;
  list->tail = NULL;
  return list;
}

void insert_sorted(struct Node *node, struct List *list) {

  struct Node *tmp = malloc(sizeof(struct Node));
  if (list->head == NULL && list->tail == NULL) {
    list->head = node;
    list->tail = node;
  }

  else {

    tmp = list->head;
    while(tmp->next != NULL && tmp->next->level < node->level){

        tmp = tmp->next;
    }

    //insert new head
    if(node->level < tmp->level){      //if new node is less than current head
        // printf("%d:%s goes in as head\n",node->level,node->path);

        node->next = list->head;
        node->prev = NULL;
        list->head->prev =node;
        list->head = node;

    }
    //insert tail
    else if (tmp->next == NULL){        // insert at tail
         //printf("%d:%s goes in as tail\n",node->level,node->path);

        node->prev = list->tail;        //new node's next is points to the current head
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;

    }
    //insert middle
    else {    //printf("%d:%s goes in as middle\n",node->level,node->path);
                                    //insertion in the middle
        node->next = tmp->next;         //new node's next points to current node's next
        node ->prev = tmp;              //new node's prev points to current node
        tmp->next->prev = node;                     //current node's next(new node's next)'s previous(
        tmp->next = node;
    }
  }
}


void insert_tail(struct Node *node, struct List *list) {
  if (list->head == NULL && list->tail == NULL) {
    list->head = node;
    list->tail = node;
  } else {
    list->tail->next = node;
    node->prev = list->tail;
    list->tail = node;
  }
}

void sub_level(struct List *list){
	int n = 1;
	int i = 0;
   	struct Node *tmpNode = list->head;
    	while(tmpNode->next != NULL){
        if(tmpNode->next->level == tmpNode->level)
            tmpNode->next->i=++n;
        else n = 1;
        i++;
        tmpNode = tmpNode->next;
    }
}

int node_swap(struct Node *tmpNode, struct List* list){
	int swap = 0;
        while(tmpNode->next != NULL ){
        	while(tmpNode->next != NULL)
                {
                    if(tmpNode->level == tmpNode->next->level && strcmp(tmpNode->path,tmpNode->next->path) > 0){
                    swap = 1;

                    char*tempPath = strdup(tmpNode->path);
                    int svalue =   tmpNode->search;


                    free(tmpNode->path);
                    tmpNode->path = strdup(tmpNode->next->path);
                    tmpNode->search = tmpNode->next->search;


                    free(tmpNode->next->path);
                    tmpNode->next->path = strdup(tempPath);
                    tmpNode->next->search = svalue;



                    free(tempPath);
                    }
                tmpNode = tmpNode->next;
                }
                if(swap == 1){
                    tmpNode = list->head;
                    swap = 0;
                }
            }
            return 0;
}

void joinThreads(LL *list){

    Node_TID *curNode = list->head;

  while(curNode->next != NULL){
        pthread_join(curNode->tid,NULL);
        curNode=curNode->next;
  }

}

void print_list(struct List *list) {
  struct Node *ptr = list->head;
  FILE *f;
  f = fopen(outputfilename, "w");
  while (ptr != NULL) {
    if (ptr != list->head) {
    }

    if(testRun == 1)printf("%d:%d:%d:%s \n" , ptr->level, ptr->i,ptr->search ,ptr->path);
    fprintf(f, "%d:%d:%d:%s\n" , ptr->level, ptr->i,ptr->search, ptr->path);
    ptr = ptr->next;
  }

 fclose(f);
}

void destroy_list(struct List *list) {
  struct Node *ptr = list->head;
  struct Node *tmp;
  while (ptr != NULL) {
    free(ptr->path);
    tmp = ptr;
    ptr = ptr->next;
    free(tmp);
  }
  free(list);
}


void open_dir(char *dir_path, struct List *list, int x){

  DIR *d;
  struct dirent *dir;
  char sub_path[255];
    int i =1;
  d = opendir(dir_path);

  if(d != NULL){

        x++;

        while ((dir = readdir(d)) != NULL) {

      	  if(dir->d_name[0] != '.') {

   	        strcpy(sub_path, dir_path);
            strcat(sub_path, "/");
            strcat(sub_path, dir->d_name);
            struct Node *tmpNode = create_node(sub_path, x, i);  //create a node for every directory
            if(ParMode == 0)
            tmpNode->search = keyword_search(sub_path,keyword);
            else{
                    pthread_t tid;
                    pthread_attr_t attr;
                    pthread_attr_init(&attr);
                    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
                    pthread_create(&tid, &attr, &makeThread, tmpNode);
                    addTID(TID_list, tid);
            }
            insert_sorted(tmpNode, list);
            if (dir -> d_type == 4 )
            open_dir(sub_path, list, x);

          }
        }
     }
    closedir(d);
}


void *makeThread(void* arg){
    struct Node *tmpNode = arg;
    tmpNode->search = keyword_search(tmpNode->path,keyword);
    pthread_exit(0);
}


int keyword_search(char * filePath,char *keyword){

    DIR *dir = opendir(filePath);
    if(dir != NULL) {
        closedir(dir);
        return 0;
    }

    FILE *file = fopen(filePath,"r");
    char buffer[1024];
    char *word;
    char *ptr=NULL;
    int line_num = 1;
    char delim[] = " \t\n";
    int count = 0;
    if(file == NULL) {
            perror("Error opening file");
            return(-1);
    }
    while( fgets (buffer, 1024, file)!=NULL ) {
        for( word=strtok_r(buffer,delim,&ptr) ; word != NULL ; word=strtok_r(NULL,delim,&ptr) ) {
            word = strdup(word);
            if(strcmp(keyword,word)==0){
                count++;
                //printf("--->Line:%d Count:%d for %s\n",line_num,count,word);
            }
            free(word);
        }
    line_num++;
    }
    //printf("File:%s \nhas %d occurance of %s\n",filePath,count,keyword);
    fclose(file);
    return count;
}



int main(int argc, char *argv[]) {

  TID_list = init_LinkedList(TID_list);

  struct List *list = create_list();
  int x = 1;
  char *path;

 if(argc < 5) {
        path = strdup("../files/final-src");
        keyword = strdup("int");
        outputfilename = strdup("TestOutput.txt");
        ParMode = 1; testRun = 1;
        printf("printing default arg\n");
 }
 else {
        path =              strdup(argv[1]);
        keyword =           strdup(argv[2]);
        outputfilename =    strdup(argv[3]);
        ParMode =           atoi(  argv[4]);
        printf("====> ArgC = %d, required: 5\n", argc);
        printf("====> Target Directory: %s\n",path);
        printf("====> Keyword: %s\n",keyword);
        printf("====> Output File-name: %s\n",outputfilename);
        if(ParMode == 1) printf("====> Parralel Search\n");
        else             printf("====> Sequential Search\n");

 }

  struct Node *tmp = create_node(path,x,1);
  tmp->search = keyword_search(path,keyword);
  insert_sorted(tmp,list);

  open_dir(path,list, x); //pass a count variable into the function so each node gets their own  variable

  if(ParMode == 1) joinThreads(TID_list);

  node_swap(list->head,list);

  sub_level(list);

  print_list(list);

  destroy_list(list);

  return 0;
}






