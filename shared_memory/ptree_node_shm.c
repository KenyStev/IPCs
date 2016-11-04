#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define SHMSZ     27

pid_t  execute(char **argU)
{
     pid_t  pid;

     if ((pid = fork()) < 0) {     /* fork a child process           */
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process:         */
          if (execvp(*argU, argU) < 0) {     /* execute the command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     }
     return pid;
}

/* argv
 * 0- name
 * 1- command (push)
 * 2- value
 * 3- key_shm (parent)
 */
int main(int argc, char const *argv[])
{
     int value=0;
     int shmid_left, shmid_right;
     key_t key_parent=0;
     char *shm_left, *shm_right, *l, *r;

     for(int i=0;i<argc;i++)
          printf("%s\n",argv[i] );
     while(1)
          sleep(1);

     return 0;
}