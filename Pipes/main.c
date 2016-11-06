#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include "fifo.h"

int menu();
int getValue();
void pressAnyKey();
void doAction();
void createNode(int);
void searchNode(int);
void deleteNode(int);

pid_t root;
int fd;
char * myfifo;

void clean_up_child_process (int signal_number)
{
    /* Clean up the child process. */
    int status;
    pid_t pid = wait(&status);

    if(pid == root){
    	root = 0;
    	printf("Before closeFifoWM: %s\n", myfifo );
    	closeFifoWM(fd, &myfifo);
    }
}

int main(){
	int menuOption;
	int nodeValue;

	printf("Process running in pid: %d\n", getpid());
	myfifo = malloc(10*sizeof(char));

	do{
		menuOption = menu();
		printf("Main menu selected option: %d\n", menuOption);
		if(menuOption != 4){
			nodeValue = getValue();
			//printf("Node Value: %d\n", nodeValue);

			doAction(menuOption, nodeValue);
			pressAnyKey();
		}
	}while(menuOption != 4);
	//closeFifoWM(fd, &myfifo);
	return 0;
}

int menu(){
	printf("Main Menu: \n");
	printf("1- Create Node\n2- Search Node\n3- Delete Node\n4- Exit\n");
	printf("Select option: ");
	int selected;
	do{
		scanf("%d", &selected);
	}while(selected <= 0 || selected >4);
	return selected;
}

int getValue(){
	int value;

	printf("Insert node value: ");
	scanf("%d", &value);

	return value;
}

void pressAnyKey(){
	sleep(1);
	printf("\nPress any key to continue...");
	getchar();
	getchar();
	printf("\n\n");
}

void doAction(int option, int nodeValue){
	//printf("- Main - DoAction - fifo_name: %s\n", myfifo);
	switch(option){
		case 1:
			createNode(nodeValue);
			break;
		case 2:
			searchNode(nodeValue);
			break;
		case 3:
			deleteNode(nodeValue);
			break;
		default:
			return;
	}
}

void createNode(int nodeValue){
	char buffer[3];
	strcpy(buffer, "");
	sprintf(buffer, "%d", nodeValue);
	if(root == 0){
		root = fork();
		if(root == -1){
			exit(0);
		}else if(root == 0){ //child
			char* params[3];
			params[0] = "./node";
			params[1] = buffer;
			params[2] = NULL;
			execv("node", params);
		}else{ //parent
			createFIFO(&fd, &myfifo, root);
			//printf("After CreateFifo: %s\n", myfifo);
			struct sigaction sigchld_action;
			memset (&sigchld_action, 0, sizeof (sigchld_action));
			sigchld_action.sa_handler = &clean_up_child_process;
			sigaction (SIGCHLD, &sigchld_action, NULL);
		}
	}else{
		char bf[10];
		strcpy(bf, "Create ");
		strcat(bf, buffer);
		sendMessage(fd, bf);
	}
}


void searchNode(int nodeValue){
	char buffer[3];
	strcpy(buffer, "");
	sprintf(buffer, "%d", nodeValue);
	char bf[10];
	strcpy(bf, "Search ");
	strcat(bf, buffer);
	sendMessage(fd, bf);
}

void deleteNode(int nodeValue){
	char buffer[3];
	strcpy(buffer, "");
	sprintf(buffer, "%d", nodeValue);
	char bf[10];
	strcpy(bf, "Delete ");
	strcat(bf, buffer);
	sendMessage(fd, bf);
}