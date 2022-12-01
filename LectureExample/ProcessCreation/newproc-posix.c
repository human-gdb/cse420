/**
 * This program forks a separate process using the fork()/exec() system calls.
 *
 * Figure 3.09
 *
 * @author Silberschatz, Galvin, and Gagne
 * Operating System Concepts  - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
  pid_t n; /* return value of fork; a process ID */

  n = fork(); /* fork a child process */

  pid_t m;

  if (n < 0) { /* error occurred */
    fprintf(stderr, "Fork Failed\n");
    exit(-1);
  } else if (n == 0) { /* child process */
    printf("I am the child with pid = %d, n = %d, m = %d\n", getpid(), n,m);
    m = fork();
    if(m==0) {
      printf("I am the grand child with pid = %d ,m = %d\n", getpid(), m);
      return execlp("/bin/ls", "ls", NULL);  
    }
    return execlp("/bin/ls", "ls", NULL);
  } else { /* parent process */
    printf("I am the parent with pid = %d, n = %d\n", getpid(), n);
    wait(NULL); /* parent will wait for the child to complete */
    printf("Child Complete\n");
    exit(0);
  }
}