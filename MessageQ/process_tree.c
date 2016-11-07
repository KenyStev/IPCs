#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msgq.c"
#include <sys/wait.h>

void showMainMenu(){
	printf("1-Create node.\n2-Delete node.\n3-Search node.\nEnter an option: ");
}
void getChoice(char* answer, int size){
    fgets(answer, size,stdin);
}

int validateChoice(char * answer, char* command){
	if(answer[0]=='1' || answer[0]=='2' || answer[0]=='3' || answer[0]=='4'){
		memcpy(command, answer, 1);
		memcpy(&command[1],"-",1);
		printf("Node num: ");
		return 1;	
	}else{
		printf("Good bye. pt\n");
		return 1;
	}
}

void validateAnswer(message_buf sbuf, Node_info* ni){
	char num[14];
	memcpy(num, &sbuf.mtext[2],14); 
	if(sbuf.mtext[0]=='1'){
		if((*ni).mkey ==0){
			createNode(num, &(*ni).mkey);
			(*ni).msgid =initQueue((*ni).mkey);
			(*ni).mynum = atoi(num);
			waitAsyncronous();
			//sendMessage(*root, sbuf);
		}else{
			sendMessage((*ni).msgid,sbuf);
			(*ni).msgid =initQueue((*ni).mkey);
		}
	}else if(sbuf.mtext[0]=='2'){
		sendMessage((*ni).msgid,sbuf);
		if((*ni).mynum == atoi(num))
			deleteChildMsgQ(ni);
		else
			(*ni).msgid =initQueue((*ni).mkey);
	}else if(sbuf.mtext[0]=='3'){
		sendMessage((*ni).msgid,sbuf);
		(*ni).msgid =initQueue((*ni).mkey);
	}else{
		printf("Error in message\n");
	}
}
void main(void){
	Node_info *ni = malloc(sizeof(Node_info));
	(*ni).mkey=0;

	printf("Process tree with message queue\n");

	int size_answer = 10;
	char *answer= malloc(size_answer);
	int exit=0;
	do{
		showMainMenu();
		message_buf sbuf;
		getChoice(answer, size_answer);
		if(validateChoice(answer, sbuf.mtext)){
			memset(answer,'\0',size_answer);
			getChoice(answer, size_answer);
			memcpy(&sbuf.mtext[2], answer, size_answer);
			printf("you entered: %s\n",sbuf.mtext);	
			validateAnswer(sbuf,ni);	
		}else
			exit=1;
	}while(!exit);
}