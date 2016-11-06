#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "fifo.h"

#define MAX_BUF 1024
int read_fd;
char * read_fifo;

void doAction(char *, char *, char *);
void createNode(char *, char *);
void searchNode(char *, char *);
void deleteNode(char *, char *);
void deleteChilds();

int value;
pid_t pidL, pidR;
int write_fdR, write_fdL;
char * write_fifoR;
char * write_fifoL;

void clean_up_child_process (int signal_number)
{
    /* Clean up the child process. */
    int status;
    pid_t pid = wait(&status);

    printf("Deleting pid: %d\n", pid);

    if(pid == pidL){
    	pidL = 0;
    	closeFifoWM(write_fdL, &write_fifoL);
    }else if(pid == pidR){
    	pidR = 0;
    	closeFifoWM(write_fdR, &write_fifoR);
    }
}

int main(int argc, char *argv[]){
	value = atoi(argv[1]);
	printf("Inserting Node with value: %d in pid: %d\n", value, getpid());
	
	char option[7];
    char nodeValue[5];
    char msg[13];
    read_fifo = malloc(10*sizeof(char));
    write_fifoR = malloc(10*sizeof(char));
    write_fifoL = malloc(10*sizeof(char));

	openFIFO(&read_fifo, &read_fd);
	//printf("After Node.openFIFO: %s\n", read_fifo);
	while(1){
		receiveMessage(read_fd, option, nodeValue, msg);
		doAction(option, nodeValue, msg);
	}
	return 0;
}

void doAction(char * option, char * nodeValue, char * message){
	if(strcmp(option, "Create") == 0){
		createNode(nodeValue, message);
	}else if(strcmp(option, "Search") == 0){
		searchNode(nodeValue, message);
	}else if(strcmp(option, "Delete") == 0){
		deleteNode(nodeValue, message);
	}
}

void deleteChilds(){
	if(pidL != 0){
		//printf("Deleting left child\n");
		char msg[] = "Delete 0"; 
		sendMessage(pidL, msg);
		sleep(1);
		closeFifoWM(pidL, &write_fifoL);
	}
	if(pidR != 0){
		//printf("Deleting right child\n");
		char msg[] = "Delete 0"; 
		sendMessage(pidR, msg);
		sleep(1);
		closeFifoWM(pidR, &write_fifoR);
	}
}

void deleteNode(char * nodeValue, char * message){
	int nv = atoi(nodeValue);
	//printf("nodeValue -> %s\n", nodeValue);
	printf("Deleting in node with value %d; finding: %d;\n", value, nv);
	printf("L: %d; R: %d;\n", pidL, pidR);
	if(nv == value || nv == 0){
		int pid = getpid();
		printf("Node found in process pid %d\n", pid);
		deleteChilds();
		sleep(1);
		closeFifoRM(read_fd);
		exit(0);
	}else if(nv < value){
		//printf("Deleting left\n");
		if(pidL == 0){
			//printf("Node not found\n");
			return;
		}else{
			sendMessage(write_fdL, message);
		}
	}else{
		//printf("Deleting right\n");
		if(pidR == 0){
			//printf("Node not found\n");
			return;
		}else{
			sendMessage(write_fdR, message);
		}
	}
}

void searchNode(char * nodeValue, char * message){
	int nv = atoi(nodeValue);
	printf("Searching in node with value %d; finding: %d;\n", value, nv);
	printf("L: %d; R: %d;\n", pidL, pidR);
	if(nv == value){
		int pid = getpid();
		printf("Node found in process pid %d\n", pid);
		return;
	}else if(nv < value){
		//printf("Searching left\n");
		if(pidL == 0){
			printf("Node not found\n");
			return;
		}else{
			sendMessage(write_fdL, message);
		}
	}else{
		//printf("Searching right\n");
		if(pidR == 0){
			printf("Node not found\n");
			return;
		}else{
			sendMessage(write_fdR, message);
		}
	}
}

void createNode(char * nodeValue, char * message){
	int nv = atoi(nodeValue);
	pid_t * actual;
	int * actual_fd;
	char * actual_fifo;
	if(nv < value){
		actual = &pidL;
		actual_fd = &write_fdL;
		actual_fifo = write_fifoL;
	}
	else{
		actual = &pidR;
		actual_fd = &write_fdR;
		actual_fifo = write_fifoR;
	}

	if(*actual == 0){
		*actual = fork();
		if(*actual  == -1){
			exit(0);
		}else if(*actual == 0){ //child
			char* params[3];
			params[0] = "./node";
			params[1] = nodeValue;
			params[2] = NULL;
			execv("node", params);
		}else{
			createFIFO(actual_fd, &actual_fifo, *actual);
			//printf("After Node.CreateFifo: %s\n", actual_fifo);
			struct sigaction sigchld_action;
			memset (&sigchld_action, 0, sizeof (sigchld_action));
			sigchld_action.sa_handler = &clean_up_child_process;
			sigaction (SIGCHLD, &sigchld_action, NULL);
		}
	}else{
		sendMessage(*actual_fd, message);
	}
	/*
	if(nv < value){
		//crear por la izquierda
		if(pidL == 0){
			pidL = fork();
			if(pidL == -1){
				exit(0);
			}else if(pidL == 0){ //child
				char* params[3];
				params[0] = "./node";
				params[1] = nodeValue;
				params[2] = NULL;
				execv("node", params);
			}else{
				createFIFO(&write_fdL, write_fifoL, pidL);
			}
		}else{
			sendMessage(write_fdL, message);
		}
	}else{
		//crear por la derecha
		if(pidR == 0){
			pidR = fork();
			if(pidR == -1){
				exit(0);
			}else if(pidR == 0){ //child
				char* params[3];
				params[0] = "./node";
				params[1] = nodeValue;
				params[2] = NULL;
				execv("node", params);
			}else{
				createFIFO(&write_fdR, write_fifoR, pidR);
			}
		}else{
			sendMessage(write_fdR, message);
		}
	}
	*/
}