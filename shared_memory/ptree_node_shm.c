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
char line[1024];
char  *argU[64];

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

     **shm = ' ';
}

void write_command_in_shm(char *shm)
{
     printf("send: %s\n", line);
     char* send = line;
     char *s = shm;
     while(*send!=0)
          *s++ = *send++;
     printf("LLEgo aqui\n");
     *s = '\0';
     printf("Paso aqui\n");
}

void push(char **argU)
{
     printf("arg: %s\n", argU[2]);
     int newVal = atoi(argU[2]);
     if(newVal<value)
     {
          if(pid_left==0)
          {
               key_left = pid_left = execute(argU);
               create_shm(&shmid_left, &shm_left,key_left);
          }else
          {
               write_command_in_shm(shm_left);
          }
     }else
     {
          if(pid_right==0)
          {
               key_right = pid_right = execute(argU);
               create_shm(&shmid_right, &shm_right, key_right);
          }else
          {
               write_command_in_shm(shm_right);
          }
     }
}

int  parse(char *line, char **argU)
{
     printf("line: \n",line);
     int cant=1;
     *argU = "./ptree_node_shm";
     *argU++;
     while (*line != '\0') {       /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
          {
                    *line++ = '\0';     /* replace white spaces with 0    */
                    cant++;
          }
          *argU++ = line;          /* save the argvent position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argvent until ...    */
     }
    *argU = '\0';                 /* mark the end of argvent list  */
     cant++;
     return cant;
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
     printf("My child's PID: %d\n", pid);
     return pid;
}

/* argv
 * 0- name
 * 1- command (push)
 * 2- value
 */
int main(int argc, char const *argv[])
{
     printf("My PID: %d\n", getpid());
     value = atoi(argv[2]);
     key_parent = getpid();
     locate_shm(&shmid,&shm,key_parent);

     while(1)
     {
          s = shm;
          while(*s == ' ')
               sleep(1);
          int i=0;
          while((*s) != 0)
               line[i++] = *s++;
          line[i] = '\0';
          s = shm;
          *s = ' ';
          
          printf("child's got: %s\n", line);

          char buffer[1024];
          memcpy(buffer,line,1024);
          parse(buffer,argU);
          printf("c1: %s\n", argU[1]);
          printf("c2: %s\n", argU[2]);
          if(strcmp(argU[1],"push")==0)
               push(argU);

     }

     return 0;
}