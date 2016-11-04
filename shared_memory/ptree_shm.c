#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define SHMSZ     128

int state=0;
int value=0;
int shmid_left, shmid_right;
key_t key_left=0, key_right=0;
pid_t pid_left, pid_right;
char *shm_left, *shm_right, *l, *r;
char  line[1024];
char  *argU[64];
int cant_parameters=0;

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

void add_parameters(char **argU, key_t base, int offset)
{
     // key_t newKey = base + offset;
     //argU+=cant_parameters;

     // int num = (offset>0)?key_right:key_left;
     // char*str = malloc(16);
     // snprintf(str, 16, "%d", num);
     // argU[cant_parameters++] = str;
     // argU[cant_parameters++] = (offset>0)?"1":"-1";
}

void push(char **argU)
{
     printf("arg: %s\n", argU[2]);
     int newVal = atoi(argU[2]);
     if(state==0){
          value = newVal;
          state=1;
     }
     else if(newVal<value)
     {
          // add_parameters(argU, key_left, -1);
          key_left = pid_left = execute(argU);
          create_shm(&shmid_left, &shm_left,key_left);
     }else
     {
          // add_parameters(argU, key_right, 1);
          key_right = pid_right = execute(argU);
          create_shm(&shmid_right, &shm_right, key_right);
     }
}

int  parse(char *line, char **argU)
{
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

void kill_child(pid_t child_pid, int sig)
{
    kill(child_pid,SIGKILL);
}

int main(int argc, char const *argv[])
{
     while(1)
     {
          printf("$ ");
          gets(line);
          cant_parameters = parse(line,argU);
          if (strcmp(argU[1], "exit") == 0)  /* is it an "exit"?     */
             exit(1);
          if (strcmp(argU[1], "push") == 0)
               push(argU);
          if (strcmp(argU[1], "write") == 0)
          {
               char* send = argU[2];
               r = shm_right;
               *r++ = '*';
               while(*send!=NULL)
                    *r++ = *send++;
               printf("LLEgo aqui\n");
               *r = '\0';
               printf("Paso aqui\n");
          }
     }    

     return 0;
}