#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
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

int main(){
	int menuOption;
	int nodeValue;

	printf("Process running in pid: %d\n", getpid());

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
	closeFifoWM(fd, myfifo);
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
			createFIFO(&fd, myfifo, root);
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
	sendMessage(fd, "Delete");
}