#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#define OPTION_SZ 6
#define MAX_BUF 1024
#define FIFO_SIZE 6

typedef struct msgqbuf
{
  long mtype;
  char mssg[20];
} mbuf_t;

sig_atomic_t child_exit_status;
struct sigaction sigchld_action;
void clean_up_child_process (int signal_number){
  /* Clean up the child process.  */
  int status;
  wait (&status);
  child_exit_status = status;
}

void waitAsync(){
  /* Handle SIGCHLD by calling clean_up_child_process.  */
  memset (&sigchld_action, 0, sizeof(sigchld_action));
  sigchld_action.sa_handler = &clean_up_child_process;
  sigaction (SIGCHLD, &sigchld_action, NULL);
}

void fork_node(char* value, pid_t* root){
	//if(*root == 0){
	*root = fork();

	if(*root == -1){
		perror("Could not fork!");
		exit(1);
	}else if(*root == 0){//child
		char* args[3];
		args[0] = "./msgchild";
		args[1] = value;
		args[2] = '\0';
		execvp("./msgchild", args);
	}else {
		waitAsync();
	}
}

void send_message(int fd, char* message){
	printf("Message to send: %s\n", message);
	write(fd, message, strlen(message) + 1);
}

void receiveMessage(int fd, char * option, char* NodeValue, char * message){
	char buf[MAX_BUF];
	
    read(fd, buf, MAX_BUF);

    char t_option[7];
    char t_nodeValue[5];

    for (int i = 0; i < 6; ++i)
    {	
    	t_option[i] = buf[i];
    	t_nodeValue[i] = buf[i+6];
    }
    strcpy(option, t_option);
    strcpy(NodeValue, t_nodeValue);
    strcpy(message, buf);
}

void createFifo(int *fd, char ** myfifo, int pid){
	sprintf(*myfifo, "%d", pid);

    mkfifo(*myfifo, 0666);

    *fd = open(*myfifo, O_WRONLY);
}

void openFifo(char ** myfifo, int *fd){
    int parentPID = getpid();
    sprintf(*myfifo, "%d", parentPID);
    *fd = open(*myfifo, O_RDONLY);
}

void closeFifoRM(int fd){
	close(fd);
}

void closeFifoWM(int fd, char ** myfifo){
	close(fd);
    unlink(*myfifo);
}

void parse_message(char* message, char* option, char* value){
	int i = 0;
	for(i = 0; i < OPTION_SZ; i++){
		option[i] = message[i];
		value[i] = message[i+OPTION_SZ];
	}
}
