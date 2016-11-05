#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define SHMSZ     128

int shmid;
key_t key=0;
pid_t pid_s=0;
char *shm;
char  line[1024];
char  *argU[64];
int cant_parameters=0;

void kill_child(pid_t child_pid)
{
    kill(child_pid,SIGKILL);
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
     // printf("arg: %s\n", argU[2]);
     int newVal = atoi(argU[2]);
     if(pid_s==0){
          key = pid_s = execute(argU);
          create_shm(&shmid, &shm,key);
     }
     else
     {
          write_command_in_shm(shm);
     }
}

void search(char **argU)
{
     // printf("arg: %s\n", argU[2]);
     int srchVal = atoi(argU[2]);
     if (pid_s>0)
     {
          write_command_in_shm(shm);
     }else{
          printf("There's no root\n");
     }
}

int main(int argc, char const *argv[])
{
     while(1)
     {
          printf("$ ");
          gets(line);
          char buffer[1024];
          memcpy(buffer,line,1024);
          cant_parameters = parse(buffer,argU);
          if (strcmp(argU[1], "exit") == 0)  /* is it an "exit"?     */
             exit(1);
          if (strcmp(argU[1], "push") == 0)
               push(argU);
          if (strcmp(argU[1], "search") == 0)
               search(argU);
     }    

     return 0;
}