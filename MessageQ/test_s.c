#include  <stdio.h>
#include <stdlib.h>
#include "msgq.c"

void main(void){
	printf("test sending\n");
	message_buf sbuf;
	strcpy(sbuf.mtext, "manana");
	sendMessage(5000,sbuf);
}