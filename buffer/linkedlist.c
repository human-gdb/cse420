#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
TO DO FOR STUDENTS: 
1) Keep track of the number of elements in the list
2) Write a find by ID function
3) Write a remove by ID function
4) Write different versions of the insert function, such as insert_sorted(), insert_after(), etc.
5) Write an insertion sort function sorting records by their IDs.
*/

struct Node {
  char *name;
  int id;
  struct Node *next;
  struct Node *prev;
};

struct List {
  struct Node *head;
  struct Node *tail;
};

struct Node *create_node(char *name, int id) {
  struct Node *node = malloc(sizeof(struct Node));
  if (node == NULL) {
    fprintf (stderr, "%s: Couldn't create memory for the node; %s\n", "linkedlist", strerror(errno));
    exit(-1);
  }
  node->name = strdup(name);
  node->id = id;
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

void print_list(struct List *list) {
  struct Node *ptr = list->head;  
  while (ptr != NULL) {
    if (ptr != list->head) {
      printf("->");
    }
    printf("(%s,%d)", ptr->name, ptr->id);
    ptr = ptr->next;
  }
  printf("\n");
}

void destroy_list(struct List *list) {
  struct Node *ptr = list->head;
  struct Node *tmp;  
  while (ptr != NULL) {
    free(ptr->name);
    tmp = ptr;
    ptr = ptr->next;
    free(tmp);
  }
  free(list);
}

/*
struct Node *find_by_id(int id, struct List *list) {

}
*/

/*
void remove_by_id(int id, struct List *list) {

}
*/

/*
void insert_sorted(struct Node *node, struct List *list) {

}
*/

/*
void insert_after_id(int id, struct Node *node, struct List *list) {

}
*/

/*
void insertion_sort_by_ID_increasing(struct List *list) {

}
*/

int main(int argc, char *argv[]) {
  struct List *list = create_list();
  struct Node *tmp = NULL;

  tmp = create_node("Kelly", 12);
  insert_tail(tmp, list);

// Add functionality to keep track of the number of elements in the list

/*
  tmp = create_node("Tom", 82);
  insert_tail(tmp, list);

  tmp = create_node("Rob", 32);
  insert_tail(tmp, list);
  
  tmp = create_node("Mary", 77);
  insert_tail(tmp, list);
  
  tmp = create_node("John", 11);
  insert_tail(tmp, list);
*/  

  printf("Unsorted List: ");
  print_list(list);

/*
  int id = 32;
  tmp = find_by_id(id, list);
  if (tmp != NULL) {
    printf ("%s is found in the list with id %d\n", tmp->name, tmp->id);
  } else {
    printf("No records exist in the list with id %d\n", id);
  }

  remove_by_id(32,list);
  print_list(list);

  remove_by_id(77,list);
  print_list(list);

  printf("Performing insertion sort...\n");
  insertion_sort_by_ID_increasing(list);

  printf("Sorted List: ");
  print_list(list);
*/

  /*Comment-out and check memory leak in valgrind*/  
  destroy_list(list);

  return 0;
}
