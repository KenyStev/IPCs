#include "utils.c"
#define OPTION_SZ 6
pid_t left_son, right_son;
int value;
int left_msgq, right_msgq;
int left_value, right_value;
int msgqid;

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
	printf("Node created with value: %d; Pid: %d;\n", value, getpid());
	mbuf_t buf;

	msgqid = connect_msgq(getpid());
	
	while(1){
		if(msgrcv(msgqid, &buf, sizeof buf.mssg, 0, 0) == -1){
			perror("msgrcv");
			exit(1);
		}
		char* option = malloc(OPTION_SZ * sizeof(char));
		char* n_value = malloc(OPTION_SZ * sizeof(char));
		parse_message(buf.mssg, option, n_value);
		do_action(option, n_value, buf.mssg);
		free(option);
		free(n_value);
	}

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
			left_msgq = create_msgq(&left_son);
		}else {
			send_message(message, &left_son, left_msgq);
		}
	}else if(val > value){
		if(right_son == 0){
			fork_node(n_value, &right_son);
			right_value = atoi(n_value);
			right_msgq = create_msgq(&right_son);
		}else {
			send_message(message, &right_son, right_msgq);
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
				send_message(message, &left_son, left_msgq);
			}else
				printf("Doesn't exist!\n");
		}else {
			if(right_son != 0){
				send_message(message, &right_son, right_msgq);
			}else
				printf("Doesn't exist!\n");
		}
	}
}

void delete_node(char* n_value, char* message){
	int val = atoi(n_value);
	if(val == value){
		delete_children();
		exit(0);	
	}else if(val < value){
		if(left_son != 0){
			send_message(message, &left_son, left_msgq);
			sleep(1);
			if(left_value == val)
				delete_left_child();
		}
	}else if(val > value){
		if(right_son != 0){
			send_message(message, &right_son, right_msgq);
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
		send_message(message, &left_son, left_msgq);	
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
		send_message(message, &right_son, right_msgq);	
		sleep(1);
		delete_right_child();
	}
}

void delete_left_child(){
	delete_msgq(left_msgq);
}

void delete_right_child(){
	delete_msgq(right_msgq);
}