#include  <stdio.h>
#include <stdlib.h>
#include "msgq.c"

void main(void){
	printf("test receiving\n");
	message_buf *sbuf= malloc(sizeof(message_buf));
	receiveMessage(5000, sbuf);
	printf("recibido: %s\n",(*sbuf).mtext);
}