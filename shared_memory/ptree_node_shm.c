#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define SHMSZ     128

int value=0;
int shmid, shmid_left, shmid_right;
key_t key_parent=0, key_left=0, key_right=0;
pid_t pid_left, pid_right;
char *shm, *shm_left, *shm_right, *s, *l, *r;

void create_shm(int *shmid,char **shm, key_t key)
{
     if ((*shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
          perror("shmget");
          exit(1);
     }

     /*
     * Now we attach the segment to our data space.
     */
     if ((*shm = shmat(*shmid, NULL, 0)) == (char *) -1) {
          perror("shmat");
          exit(1);
     }
}

void locate_shm(int *shmid,char **shm, key_t key)
{
     if ((*shmid = shmget(key, SHMSZ, 0666)) < 0) {
          perror("shmget");
          exit(1);
     }

     /*
     * Now we attach the segment to our data space.
     */
     if ((*shm = shmat(*shmid, NULL, 0)) == (char *) -1) {
          perror("shmat");
          exit(1);
     }
}

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
 * //3- key_shm (parent)
 * //4- left(-1)/right(1)
 */
int main(int argc, char const *argv[])
{
     printf("My PID: %d\n", getpid());
     value = atoi(argv[2]);
     key_parent = getpid();
     locate_shm(&shmid,&shm,key_parent);

     while(1)
     {
          int i;
          char line[1024];
          s = shm;
          while(*s == ' ')
               sleep(1);
          for (s = shm,i=0; (*s) != 0; s++,i++)
               line[i] = *s;
          line[i] = '\0';
          s = shm;
          *s = ' ';
          printf("child's got: %s\n", line);
     }

     return 0;
}