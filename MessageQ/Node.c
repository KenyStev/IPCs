#include <stdio.h>
#include <stdlib.h>
#include "msgq.c"


void parse(const char* msg, int* option, char* number){
	char op[1];
	op[0] = msg[0];
	*option = atoi(op);
	memcpy(number, &msg[2],14);
}

void sendMsgChild(Node_info* node,message_buf  *sbuf){
	if((*node).mkey!=0){
		sendMessage((*node).msgid,*sbuf);
		(*node).msgid =initQueue((*node).mkey);
		if((*sbuf).mtext[0]=='4'){
			deleteChildMsgQ(node);
		}
	}else
		printf("Nodo.mkey =0\n");
}

void validateMsg(int option, char* c_number, Node_info* l_node,Node_info* r_node,int my_num, message_buf  *sbuf){
	int number = atoi(c_number);
	switch(option){
		case 1:
			if(number<my_num){
				if((*l_node).mkey==0){
					printf("crear hijo izquierdo con %d\n",number);
					createNode(c_number, &(*l_node).mkey);
					(*l_node).msgid =initQueue((*l_node).mkey);
					(*l_node).mynum = atoi(c_number);
					waitAsyncronous();
				}else{
					printf("envio izq\n");
					sendMessage((*l_node).msgid,*sbuf);
					(*l_node).msgid =initQueue((*l_node).mkey);
				}
			}else{
				if((*r_node).mkey==0){
					printf("crear hijo derecho con %d\n",number);
					createNode(c_number, &(*r_node).mkey);
					(*r_node).msgid =initQueue((*r_node).mkey);
					(*r_node).mynum = atoi(c_number);
					waitAsyncronous();
				}else{
					printf("envio der\n");
					sendMessage((*r_node).msgid,*sbuf);
					(*r_node).msgid =initQueue((*r_node).mkey);
				}
			}
			break;
		case 2:
				printf("entro en caso 2 en nodo: %d and number: %d\n",my_num, number );
				if(number == my_num){
					printf("Encontrado nodo a eliminar\n");
					memset((*sbuf).mtext,0,MSGSZ);
					strcpy((*sbuf).mtext,"4-1");
					sendMsgChild(l_node, sbuf);
					sendMsgChild(r_node, sbuf);
					exit(0);
				}else if(number< my_num){
					if((*l_node).mkey!=0){
						sendMessage((*l_node).msgid,*sbuf);
						if((*l_node).mynum==number){
							deleteChildMsgQ(l_node);
						}else
							(*l_node).msgid =initQueue((*l_node).mkey);
						
					}
				}else{
					if((*r_node).mkey!=0){
						sendMessage((*r_node).msgid,*sbuf);
						if((*r_node).mynum==number){
							deleteChildMsgQ(r_node);
						}
						else{
							(*r_node).msgid =initQueue((*r_node).mkey);
						}
					}
				}
			break;
		case 3:
			if(number == my_num){
				printf("Nodo encontrado en pid: %d\n",getpid());
			}else if(number< my_num){
				if((*l_node).mkey!=0){
					sendMessage((*l_node).msgid,*sbuf);
					(*l_node).msgid =initQueue((*l_node).mkey);
				}
			}else{
				if((*r_node).mkey!=0){
					sendMessage((*r_node).msgid,*sbuf);
					(*r_node).msgid =initQueue((*r_node).mkey);
				}
			}
			break;
		case 4:
				printf("Entro en caso 4\n");
				sendMsgChild(l_node, sbuf);
				sendMsgChild(r_node, sbuf);
				exit(0);
			break;
		default:
			printf("Invalid option: %d\n",option);
	}	
}
int main(int argc, char* argv[]){
	int pid = getpid();
	Node_info *l_ni = malloc(sizeof(Node_info));
	(*l_ni).mkey=0;
	Node_info *r_ni = malloc(sizeof(Node_info));
	(*r_ni).mkey=0;
	
	int num = atoi(argv[1]);
	printf("mi num es: %d\n",num);
    message_buf  *sbuf= malloc(sizeof(message_buf));
    int option =0;
    char* child_num=malloc(14);
    do{
    	memset((*sbuf).mtext,0,MSGSZ);
    	memset(child_num,0,14);
    	//printf("llamo a rcvmsg el %d\n",num );
    	receiveMessage(pid, sbuf,num);
    	parse((*sbuf).mtext,&option, child_num);
    	validateMsg(option,child_num,l_ni,r_ni,num,sbuf);
    }while(option!=4);
    printf("salio num: %d\n",num);
	return 0;
}