#include "utils.c"
#define OPTION_SZ 6
pid_t left_son, right_son;
int value;
int left_fd, right_fd;
char *left_fifo, *right_fifo;
int left_value, right_value;
int fd;
char * parent_fifo;

void do_action(char*, char*, char*);
void create_node(char*, char*);
void search_node(char*, char*);
void delete_node(char* n_value, char* message); 
void delete_children();
void delete_left_child();
void delete_right_child();

int main(int argc, char *argv[])
{
	value = atoi(argv[1]);
	parent_fifo = malloc(FIFO_SIZE * sizeof(char));
	left_fifo = malloc(FIFO_SIZE * sizeof(char));
	right_fifo = malloc(FIFO_SIZE * sizeof(char));

	openFifo(&parent_fifo, &fd);

	char * message = malloc(MAX_BUF * sizeof(char));
	char * option = malloc(MAX_BUF * sizeof(char));
	char * n_value = malloc(MAX_BUF * sizeof(char));
	
	while(1){
		bzero(message, MAX_BUF);
		bzero(option, MAX_BUF);
		bzero(n_value, MAX_BUF);
		receiveMessage(fd, option, n_value, message);
		do_action(option, n_value, message);
	}

	free(message);
	free(n_value);
	free(option);

	return 0;
}

void do_action(char* option, char* n_value, char* message){
	switch(option[0]){
		case 'c': //create
			create_node(n_value, message);
			break;
		case 's': //search
			search_node(n_value, message);
			break;
		case 'd': //delete
			delete_node(n_value, message);
			break;
	}
}

void create_node(char* n_value, char* message){
	int val = atoi(n_value);
	
	if(val < value){
		if(left_son == 0){
			fork_node(n_value, &left_son);
			left_value = atoi(n_value);
			createFifo(&left_fd, &left_fifo, left_son);
		}else {
			send_message(left_fd ,message);
		}
	}else if(val > value){
		if(right_son == 0){
			fork_node(n_value, &right_son);
			right_value = atoi(n_value);
			createFifo(&right_fd, &right_fifo, right_son);
		}else {
			send_message(right_fd ,message);
		}
	}	
}

void search_node(char* n_value, char* message){
	int val = atoi(n_value);
	if(val == value){
		printf("Found value: %s\n", n_value);
	}else{
		if(val < value){
			if(left_son != 0){
				send_message(left_fd ,message);
			}else
				printf("Doesn't exist!\n");
		}else {
			if(right_son != 0){
				send_message(right_fd ,message);
			}else
				printf("Doesn't exist!\n");
		}
	}
}

void delete_node(char* n_value, char* message){
	int val = atoi(n_value);
	if(val == value){
		delete_children();
		closeFifoRM(fd);
		exit(0);	
	}else if(val < value){
		if(left_son != 0){
			send_message(left_fd ,message);
			sleep(1);
			if(left_value == val)
				delete_left_child();
		}
	}else if(val > value){
		if(right_son != 0){
			send_message(right_fd ,message);
			sleep(1);
			if(right_value == val)
				delete_right_child();
		}
	}
}

void delete_children(){
	if(left_son != 0){
		char* message = malloc(OPTION_SZ * 2 * sizeof(char));
		strcpy(message, "delete");
		char* val = malloc(OPTION_SZ * sizeof(char));
		sprintf(val, "%d", left_value);
		strcat(message, val);
		printf("My nuovo val: %s\n", message);
		send_message(left_fd, message);	
		sleep(1);
		delete_left_child();
	}
	if(right_son != 0){
		char* message = malloc(OPTION_SZ * 2 * sizeof(char));
		strcpy(message, "delete");
		char* val = malloc(OPTION_SZ * sizeof(char));
		sprintf(val, "%d", right_value);
		strcat(message, val);
		printf("My nuovo val: %s\n", message);
		send_message(right_fd ,message);	
		sleep(1);
		delete_right_child();
	}
}

void delete_left_child(){
	closeFifoWM(left_fd, &left_fifo);
}

void delete_right_child(){
	closeFifoWM(right_fd, &right_fifo);
}