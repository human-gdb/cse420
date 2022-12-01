#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

//#include "mythreads.h"

#define CMAX (10)
#define MAXOUTSIZE 1024+1+256

int max;
int items;
char **buffer;

int use  = 0;
int fill = 0;

sem_t empty;
sem_t full;
sem_t mutex;


int consumers = 1;

void do_fill(int value) {
  sprintf(buffer[fill],"%d:STRING", value) ;
  fill++;
  if (fill == max) {
    fill = 0;
  }
}

char* do_get() {
  char* tmp = strdup(buffer[use]);
  use++;
  if (use == max) {
    use = 0;
  }
  return tmp;
}

void * producer(void *arg) {
  int i;
  for (i = 0; i < items; i++) {
    sem_wait(&empty);
    sem_wait(&mutex);
    do_fill(i);
    sem_post(&mutex);
    sem_post(&full);
    printf("Producer - Item: %d is inserted\n", i);
  }


  // end case
  for (i = 0; i < consumers; i++) {
    sem_wait(&empty);
    sem_wait(&mutex);
    do_fill(-1);
    sem_post(&mutex);
    sem_post(&full);
  }

  return NULL;
}

void * consumer(void *arg) {
  char *tmp = NULL;
  while (strcmp(tmp,"-1:STRING") != 0) {
    sem_wait(&full);
    sem_wait(&mutex);
    tmp = do_get();
    sem_post(&mutex);
    sem_post(&empty);
    if (strcmp(tmp,"-1:STRING") != 0) {
      printf("Consumer%d - Item: %s is extracted.\n", (*(int *)arg), tmp);
    }
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "usage: %s <buffersize> <items> <consumers>\n", argv[0]);
    exit(1);
  }
  max   = atoi(argv[1]);
  items = atoi(argv[2]);
  consumers = atoi(argv[3]);
  assert(consumers <= CMAX);

  buffer[MAXOUTSIZE] = calloc(max,MAXOUTSIZE));
  int i, con_ids[CMAX];

  for (i = 0; i < max; i++) {
    buffer[i] = 0;
  }

  sem_init(&empty, 0, max); // max are empty
  sem_init(&full, 0, 0);    // 0 are full
  sem_init(&mutex, 0, 1);   // mutex

  pthread_t pid, cid[CMAX];

  pthread_create(&pid, NULL, producer, NULL);
  for (i = 0; i < consumers; i++) {
    con_ids[i] = i;
    pthread_create(&cid[i], NULL, consumer, (con_ids+i));
  }

  pthread_join(pid, NULL);
  for (i = 0; i < consumers; i++) {
    pthread_join(cid[i], NULL);
  }

  return 0;
}
