#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define OPTION_SZ 6

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

void send_message(char* message, int* key, int msgq_id){
	mbuf_t msg_buf;
	msg_buf.mtype = 2;
	bzero(msg_buf.mssg, 20);
	//msg_buf.mssg = message;
	strcpy(msg_buf.mssg, message);
	int len = strlen(msg_buf.mssg);

	if (msgsnd(msgq_id, &msg_buf, len+1, 0) == -1)
		perror("msgsnd");
}

int create_msgq(int* key){
	int msgqid = msgget(*key, 0644 | IPC_CREAT);

	if(msgqid == -1) {
		perror("msgget");
		exit(1);
	}

	return msgqid;
}

int connect_msgq(int key){
	int msgqid;
	if ((msgqid = msgget(key, 0644)) == -1) { /* connect to the queue */
		perror("msgget");
		exit(1);
	}

	return msgqid;
}

void parse_message(char* message, char* option, char* value){
	int i = 0;
	for(i = 0; i < OPTION_SZ; i++){
		option[i] = message[i];
		value[i] = message[i+OPTION_SZ];
	}
}

void delete_msgq(int msgqid){
	return;
	if (msgctl(msgqid, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(1);
	}
	printf("msgq deleted %d\n", msgqid);
}