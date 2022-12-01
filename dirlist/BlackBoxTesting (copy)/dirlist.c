#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>



//create node structure
struct Node {
  char *path;
  int level;
  int i;
  struct Node *next;
  struct Node *prev;
};

//create list structure
struct List {
  struct Node *head;
  struct Node *tail;
};

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

    struct Node *tmp = list->head;
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

void NumberChildren(struct List *list){
    int inum = 1;
    int i=0;

    struct Node *curNode = list->head;
    while(curNode->next != NULL){
        if(curNode->next->level == curNode->level)
            curNode->next->i=++inum;
        else inum=1;
        i++;
        curNode = curNode->next;
    }
}

int SortHelper(struct Node *curNode, struct List* list){

int Swap = 0;

            while(curNode->next != NULL ){

              while(curNode->next != NULL)
                {
                    if(curNode->level == curNode->next->level && strcmp(curNode->path,curNode->next->path) > 0){
                    Swap=1;
                    char*tempPath = strdup(curNode->path);
                    free(curNode->path);
                    curNode->path = strdup(curNode->next->path);
                    free(curNode->next->path);
                    curNode->next->path = strdup(tempPath);
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

void print_listsorted(struct List *list) {
  struct Node *ptr = list->head;
  while (ptr != NULL) {
    if (ptr != list->head) {
      printf("\n");
    }
    printf("%d:%d%s", ptr->level,ptr->level, ptr->path);
//    fprintf(f, "%s (Level %d)\n" , ptr->path, ptr->level);
    ptr = ptr->next;
  }
  printf("\n");
}

void print_list(struct List *list) {
  struct Node *ptr = list->head;
  FILE *f;
  f = fopen("output2.txt", "w");
  while (ptr != NULL) {
    if (ptr != list->head) {
    }
    printf("%d:%d %s \n" , ptr->level, ptr->i, ptr->path);

    fprintf(f, "%d:%d %s\n" , ptr->level, ptr->i, ptr->path);
    ptr = ptr->next;
  }
  printf("\n");
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
            insert_sorted(tmpNode, list);
            if (dir -> d_type == 4 )
            open_dir(sub_path, list, x);

          }
        }
     }
    closedir(d);
}


int main(int argc, char *argv[]) {

  struct List *list = create_list();
  int x = 1;
  char *str;

 if(argc < 2) {
 printf("Printing with  default args\n");
        str = strdup("/home/darren/Downloads/final-src");
        outputfilename = strdup("TestOutput.txt");
 }
 else {
 printf("Printing with  command-line args\n");
        str = strdup(argv[1]);
        outputfilename = strdup(argv[2]);

 }

  struct Node *tmp = create_node(str,x,1);
  insert_sorted(tmp,list);

  open_dir(str,list, x); //pass a count variable into the function so each node gets their own  variable


// print_list(list);
  SortHelper(list->head,list);
  NumberChildren(list);

//  print_list(list);


  destroy_list(list);

  return 0;
}

