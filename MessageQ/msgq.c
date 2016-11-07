#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#define MSGSZ	16

typedef struct msgbuf {
         long    mtype;
         char    mtext[MSGSZ];
         } message_buf;

typedef struct nInfo {
	int mkey;
	int msgid;
	int mynum;
} Node_info;

int initQueue(int mkey){
	int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key =mkey;
	if ((msqid = msgget(key, msgflg )) < 0) {
		perror("msgget");
		exit(1);
    }

 	return msqid;

}
void sendMessage(int msqid,message_buf sbuf){
	size_t buf_length;
 	sbuf.mtype = 1;
 	buf_length = strlen(sbuf.mtext) + 1 ;
 	if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
       printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buf_length);
        perror("msgsnd");
        exit(1);
    }
   else 
      printf("Message: \"%s\" Sent\n", sbuf.mtext);
}

void receiveMessage(int mkey, message_buf* rbuf, int nodo){
	int msqid;
	key_t key =mkey;
    if ((msqid = msgget(key, 0666)) < 0) {
    	perror("msgget");
    	exit(1);
    }
    printf("1.msqid:%d, rbuf: %s nodo:%d\n",msqid, (*rbuf).mtext,nodo);
    
    if (msgrcv(msqid, rbuf, MSGSZ, 1, 0) < 0) {
    	if(errno ==	EINTR){
    		//printf("Not going to crash!\n");
    		return;
    	}
    	else{
	    	printf("msqid:%d, rbuf: %s nodo:%d\n",msqid, (*rbuf).mtext,nodo);
	    	perror("msgrcv");
	    	exit(1);
    	}
    }
}

void createNode(char* num, int* root){
	pid_t  pid;
	pid = fork();
	if(pid == 0){
		char *t[3];
		t[0]="./Node";
		t[1]=num;
		t[2]=0; 
		execvp ("./Node", t);
	}else {
		*root = pid;
	}
}

void deleteChildMsgQ(Node_info* ni){
	int key, mask, msgid;
	key = (*ni).mkey;
	mask = 0666;
	msgid = msgget(key, mask);	

	if (msgid == -1) {
		printf("Message queue does not exist.\n");
		exit(EXIT_SUCCESS);
	}

	if (msgctl(msgid, IPC_RMID, NULL) == -1) {
		fprintf(stderr, "Message queue could not be deleted.\n");
		exit(EXIT_FAILURE);
	}

	printf("Message queue was deleted.\n");
	(*ni).mkey=0;
	(*ni).mynum=0;
}

sig_atomic_t child_exit_status;
struct sigaction sigchld_action;

void clean_up_child_process (int signal_number)
{
	/* Clean up the child process.  */
	int status;

	int pid=wait (&status);
	printf("pid eliminado:%d status:%d\n",pid,status);
	/* Store its exit status in a global variable.  */
	child_exit_status = status;
}

void waitAsyncronous(){
	memset (&sigchld_action, 0, sizeof (sigchld_action));
	sigchld_action.sa_handler = &clean_up_child_process;
	sigaction (SIGCHLD, &sigchld_action, NULL);
}

