#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>

#define MAX_PATH_SIZE 256

typedef struct Item Item;
typedef struct Node Node;
typedef struct SDLL SDLL;
typedef struct Node_TID Node_TID;
typedef struct LL LL;

char *cpath,*abspath;
int level = 0;
int printDirHelper(char *cpath,int clevel,SDLL *list);
int printNodes(Node *node);
int SortHelper(Node *curNode,SDLL* list);
void SwapItem(Node* n1, Node *n2);
void printDir(char *path,SDLL* list,int lvl);
SDLL *init_SortedDoublyLinkedList(SDLL *list);
int search_test(char * filePath,char *keyword);
char *rws(char *str);
char *keyword;
int totalMatches = 0;
enum parbool{PARALLEL=1,SEQUENTIAL=0};
enum parbool runMode;
LL *TID_list;
void printTIDs(Node_TID *node);


        char delim[] = " \t\n";


        int search_test(char * filePath,char *keyword){

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

        int count = 0;
        if(file == NULL) {
              perror("Error opening file");
              return(-1);
           }

           while( fgets (buffer, 1024, file)!=NULL ) {

              for(word=strtok_r(buffer,delim,&ptr);word != NULL;word=strtok_r(NULL,delim,&ptr) ) {
                  word = strdup(rws(word));

                  if(strcmp(keyword,word)==0){
                        count++;
                        totalMatches++;
                        //printf("--->Line:%d Count:%d for %s\n",line_num,count,word);
                  }

              }
           line_num++;
           }
           //printf("File:%s \nhas %d occurance of %s\n",filePath,count,keyword);
           fclose(file);
         return count;
        }
char *rws(char *str)
{
	int i = 0, j = 0;
	while (str[i])
	{
		if (str[i] != ' ')
          str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
	return str;
}


typedef struct Item{
    int level;
    int num;
    int count;
    char * path;
}Item;

typedef struct Node {
    Node* prev;
    Item* item;
    Node* next;
}Node;

typedef struct SDLL{
    Node* head;
    Node* tail;
    int size;
}SDLL;

typedef struct Node_TID {
    pthread_t tid;
    Node_TID *next;
}Node_TID;

typedef struct LL{
    Node_TID* head;
    Node_TID* tail;
    int size;
}LL;

LL* init_LinkedList(LL *list){
    list = malloc(sizeof(struct LL));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;

}



void addTID(LL* list,pthread_t tid){

    Node_TID* newTID = malloc(sizeof(struct Node_TID));
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

Item* AddPath(SDLL *list,int level,char *path){
list->size++;
Item* newitem = malloc(sizeof(struct Item));
newitem->level = level;
newitem->num = 1;  //incremented by calling number children
newitem->path = strdup(path);



Node* newNode = malloc(sizeof(struct Node));
newNode->item = newitem;

    if(list->head == NULL && list->tail==NULL){
        newNode->next=NULL;
        newNode->prev=NULL;
        list->head = newNode;
        list->tail = newNode;
    }
    else{
            // 3 cases: new path has lesser level, same level, higher level

        Node *curNode = list->head;
        while(curNode->next != NULL && curNode->next->item->level < newitem->level){
            curNode = curNode->next;
        }

        if(curNode->item->level > newitem->level){ //added as head, no lesser level found
            newNode->next = list->head;
            newNode->prev = NULL;
            list->head->prev = newNode;
            list->head = newNode;
        }
        else if(curNode->next == NULL){ //added as a new tail , no higher level found
          newNode->prev = list->tail;
          newNode->next = NULL;
          list->tail->next = newNode;
          list->tail = newNode;
        }
        else{ //added as middle link
                newNode->next = curNode->next;
                newNode->prev = curNode;
                curNode->next->prev = newNode;
                curNode->next = newNode;
        }
    }
    return newitem;
}

SDLL* init_SortedDoublyLinkedList(SDLL *list){
    list = malloc(sizeof(struct SDLL));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;

}

void destroySDLL(SDLL *list){
    Node *n = list->head;
    Node *temp;
    while(n!=NULL){
        free(n->item->path);
        free(n->item);
        temp = n;
        n=n->next;
        free(temp);
    }
    free(list);
    free(n);
}


void destroyLL(LL *list){
    Node_TID *n = list->head;
    Node_TID *temp;
    while(n!=NULL){
        temp = n;
        n=n->next;
        free(temp);
    }
    free(list);
    free(n);
}



void printSDLL(SDLL *list){
    printNodes(list->head);
}

int printNodes(Node *node){
    printf("    %d:%d:%d:%s\n",node->item->level,node->item->num, node->item->count , node->item->path );
    if(node->next != NULL) return printNodes(node->next); else return 0;
}


void printDirSeq(char *path,SDLL* list,int lvl){
    DIR *folder = opendir(path);
    struct dirent *entry;

    if(lvl==1)AddPath(list,1,path)->count = search_test(path,keyword);
    int level = ++lvl;

    if(folder == NULL){
        perror("Unable to open dir");
        exit(-1);
    }

    while( (entry=readdir(folder)) ){
        if(entry->d_name[0] != '.' ){
                //printf("%d:%s/%s\n",level,path,entry->d_name);
                abspath = malloc(MAX_PATH_SIZE);
                sprintf(abspath,"%s/%s",path,entry->d_name);
                AddPath(list,level,abspath)->count = search_test(abspath,keyword);
                free(abspath);
        }
        if(entry->d_name[0] != '.'&& entry->d_type == 4){
            char* chpath = malloc(strlen(path)+strlen(entry->d_name)+3);
            sprintf(chpath,"%s/%s",path,entry->d_name);
            printDirSeq(chpath,list,level);
            free(chpath);
        }

    }closedir(folder);free(entry);
}


void* StartSearch(void *param){
    Item *item = (Item*)param;
    item->count = search_test(item->path,keyword);
    pthread_exit(0);
}

void printDirPar(char *path,SDLL* list,int lvl){
    DIR *folder = opendir(path);
    struct dirent *entry;
    pthread_t tid; /* the thread ID */
    pthread_attr_t attr;

    if(lvl==1)AddPath(list,1,path)->count = 0;
    int level = ++lvl;

    if(folder == NULL){
        perror("Unable to open dir");
        exit(-1);
    }

    while( (entry=readdir(folder)) ){
        if(entry->d_name[0] != '.' ){
                //printf("%d:%s/%s\n",level,path,entry->d_name);
                abspath = malloc(MAX_PATH_SIZE);
                sprintf(abspath,"%s/%s",path,entry->d_name);
                Item *itemAdr = AddPath(list,level,abspath); itemAdr->count = 69;
                  pthread_attr_init(&attr);
                  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
                  /* create the thread */
                  pthread_create(&tid, &attr, StartSearch, itemAdr);
                  addTID(TID_list, tid);
                //}
                free(abspath);
        }
        if(entry->d_name[0] != '.'&& entry->d_type == 4){
            char* chpath = malloc(strlen(path)+strlen(entry->d_name)+3);
            sprintf(chpath,"%s/%s",path,entry->d_name);
            printDirPar(chpath,list,level);
            free(chpath);
        }

    }closedir(folder);free(entry);
}




void SortChildren(SDLL *list){
    SortHelper(list->head,list);
}

int SortHelper(Node *curNode,SDLL* list){

int Swap = 0;

            while(curNode->next != NULL ){

              while(curNode->next != NULL)
                {
                    if(curNode->item->level == curNode->next->item->level && strcmp(curNode->item->path,curNode->next->item->path) > 0){
                    Swap=1;
                    char*tempPath = strdup(curNode->item->path);
                    int tempCount = curNode->item->count;
                    free(curNode->item->path);
                    curNode->item->path = strdup(curNode->next->item->path);
                    curNode->item->count = curNode->next->item->count;
                    free(curNode->next->item->path);
                    curNode->next->item->path = strdup(tempPath);
                    curNode->next->item->count = tempCount;
                    free(tempPath);
                    }

                    curNode = curNode->next;
                }

                if(Swap == 1){
                    curNode = list->head;
                    Swap=0;
                }

            }
            return 0;
}

void NumberChildren(SDLL *list){
    int inum = 1;
    int i=0;

    Node *curNode = list->head;
    while(i<list->size-1){
        if(curNode->next->item->level == curNode->item->level)
            curNode->next->item->num=++inum;
        else inum=1;
        i++;
        curNode = curNode->next;
    }
}



void fileWriter(SDLL*list,FILE* fp){
    if(fp==NULL){
        perror("Unable export output.txt");
        exit(-1);
    }
    Node*node=list->head;
    while(node != NULL){
        fprintf(fp,"%d:%d:%d:%s\n",node->item->level,node->item->num,node->item->count,node->item->path);
        node=node->next;
    }
    fclose(fp);
    free(node);
}

void MakeAllThreadsJoin(LL *list){

    Node_TID *curNode = list->head;

  while(curNode->next != NULL){
        pthread_join(curNode->tid,NULL);
        curNode=curNode->next;
  }

}
int main(int argc, char *argv[]){

    char *testpath;
    FILE *file;
    TID_list = init_LinkedList(TID_list);

    if(argc < 5){
        testpath= strdup("/home/alex/cse420/pardirlist/BlackBoxTesting/files");
        file   = fopen("output.txt","w");
        keyword = strdup("int");
        runMode = SEQUENTIAL;
    }
    else{
        testpath = strdup(argv[1]);
        keyword = strdup(argv[2]);
        file   = fopen(argv[3],"w");
        runMode = atoi(argv[4]);
    }

    SDLL *mylist = NULL;
    mylist =  init_SortedDoublyLinkedList(mylist);


    if(runMode == PARALLEL){
        printDirPar(testpath,mylist,1);
        MakeAllThreadsJoin(TID_list);
    }
    else
        printDirSeq(testpath,mylist,1);





    SortChildren(mylist);
    NumberChildren(mylist);



   // printf("\n\nSorted Doubly linked list\n\n");
   // printSDLL(mylist);

    fileWriter(mylist,file);

    if(argc==3) printf("\nStarting from Parent Directory:%s\n%s file has been exported with %d directories, %d Matches for %s .\n\n",argv[1],argv[2],mylist->size,totalMatches,keyword);
    else        printf("\nTest directory [%s] exported to: \nFile: %s , %d directories, %d Matches for %s \n",testpath,argv[3],mylist->size,totalMatches,keyword);

    destroySDLL(mylist);
    destroyLL(TID_list);
    free(testpath);
    return 0;
}
