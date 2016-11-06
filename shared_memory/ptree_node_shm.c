#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define SHMSZ     128

int value=0;
sig_atomic_t child_exit_status;
int shmid, shmid_left, shmid_right;
key_t key_parent=0, key_left=0, key_right=0;
pid_t pid_left, pid_right;
char *shm, *shm_left, *shm_right, *s, *l, *r;
char line[1024];
char  *argU[64];
/* Handle SIGCHLD by calling clean_up_child_process.  */
struct sigaction sigchld_action;

void clean_up_child_process (int signal_number)
{
     /* Clean up the child process.  */
     int status;
     pid_t pid = wait (&status);
     printf("PID killed: %d\n", pid);
     /* Store its exit status in a global variable.  */
     child_exit_status = status;

     if(pid==pid_left)
          pid_left=0;
     else if(pid==pid_right)
          pid_right=0;
}

void wait_async()
{
     memset (&sigchld_action, 0, sizeof (sigchld_action));
     sigchld_action.sa_handler = &clean_up_child_process;
     sigaction (SIGCHLD, &sigchld_action, NULL);
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
     *s = '\0';
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
               wait_async();
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
               wait_async();
          }else
          {
               write_command_in_shm(shm_right);
          }
     }
}

void search(char **argU)
{
     printf("arg S: %s\n", argU[2]);
     int srchVal = atoi(argU[2]);
     if (value==srchVal)
     {
          printf("PID of value(%d): %d\n", srchVal, getpid());
     }else{
          if(pid_left!=0)
               write_command_in_shm(shm_left);
          if(pid_right!=0)
               write_command_in_shm(shm_right);
     }
}

void kill_children()
{
     memcpy(line,"kill_children",64);
     if(pid_left>0)
     {
          write_command_in_shm(shm_left);
     }
     if(pid_right>0)
     {
          write_command_in_shm(shm_right);
     }
     exit(0);
}

void kill_child(char **argU)
{
     int killVal = atoi(argU[2]);
     if (value==killVal)
     {
          kill_children();
     }else if(killVal<value){
          write_command_in_shm(shm_left);
     }else{
          write_command_in_shm(shm_right);
     }
}

/* argv
 * 0- name
 * 1- command (push)
 * 2- value
 */
int main(int argc, char const *argv[])
{
     // printf("My PID: %d\n", getpid());
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
          
          if(strcmp(argU[1],"push")==0)
               push(argU);
          if(strcmp(argU[1], "search")==0)
               search(argU);
          if (strcmp(argU[1], "kill_children") == 0)
               kill_children();
          if (strcmp(argU[1], "kill") == 0)
               kill_child(argU);

     }

     return 0;
}