#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH_SIZE 256

typedef struct Item Item;
typedef struct Node Node;
typedef struct SDLL SDLL;
char *cpath,*abspath;
int level = 0;
int printDirHelper(char *cpath,int clevel,SDLL *list);
int printNodes(Node *node);
int SortHelper(Node *curNode,SDLL* list);
void SwapItem(Node* n1, Node *n2);


typedef struct Item{
    int level;
    int num;
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

void AddPath(SDLL *list,int level,char *path){
list->size++;
Item* newitem = malloc(sizeof(struct Item));
newitem->level = level;
newitem->num = 1;  //incremented during insertion
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

        if(curNode->item->level > newitem->level){ //added as , no lesser level found
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
}

void destroyList(SDLL *list){
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

void printSDLL(SDLL *list){
    printNodes(list->head);
}

int printNodes(Node *node){
    printf("    %d:%d:%s\n",node->item->level,node->item->num,node->item->path);
    if(node->next != NULL) return printNodes(node->next); else return 0;
}

void printDir(char *path,SDLL* list,int lvl){
    DIR *folder = opendir(path);
    struct dirent *entry;

    if(lvl==1)AddPath(list,1,path);
    int level = ++lvl;

    if(folder == NULL){
        perror("Unable to open dir");
        exit(-1);
    }

    while( (entry=readdir(folder)) ){
        if(entry->d_name[0] != '.' ){
                printf("%d:%s/%s\n",level,path,entry->d_name);
                abspath = malloc(MAX_PATH_SIZE);
                sprintf(abspath,"%s/%s",path,entry->d_name);
                AddPath(list,level,abspath);
                free(abspath);
        }
        if(entry->d_name[0] != '.'&& entry->d_type == 4){
            char* chpath = malloc(strlen(path)+strlen(entry->d_name)+3);
            sprintf(chpath,"%s/%s",path,entry->d_name);
            printDir(chpath,list,level);
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
                    free(curNode->item->path);
                    curNode->item->path = strdup(curNode->next->item->path);
                    free(curNode->next->item->path);
                    curNode->next->item->path = strdup(tempPath);
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
        fprintf(fp,"%d:%d:%s\n",node->item->level,node->item->num,node->item->path);
        node=node->next;
    }
    fclose(fp);
    free(node);
}

int main(int argc, char *argv[]){

    char *testpath = malloc(256);
    FILE *file;

    if(argc < 3){
        strcpy( testpath,"BlackBoxTesting/files/linux-master");
        file   = fopen("output.txt","w");
    }
    else{
        strcpy(testpath,argv[1]);
        file   = fopen(argv[2],"w");
    }

    SDLL *mylist = malloc(sizeof(struct SDLL));
    mylist->head = NULL;
    mylist->tail = NULL;
    mylist->size = 0;



    printf("\n\nRaw Directory paths\n\n");
    printDir(testpath,mylist,1);

    printf("\n\nMy Doubly linked list\n\n");
    printSDLL(mylist);

    SortChildren(mylist);
    NumberChildren(mylist);

    printf("\n\nSorted Doubly linked list\n\n");
    printSDLL(mylist);

    fileWriter(mylist,file);

    if(argc==3) printf("%s file is been exported, with %d lines",argv[2],mylist->size);

    destroyList(mylist);free(testpath);
    return 0;
}
