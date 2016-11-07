#include "utils.c"

pid_t root = 0;
int root_value;
int msgq_id;
int show_menu_get_answer(){
  printf("1. Create process.\n2. Search process.\n3. Delete process.\nAnswer: ");
  int answer;
  scanf("%d", &answer);
  
  return answer;
}

void main_create();
void main_search();
void main_delete();
void get_value(char*);
//void main_delete();

int main(int argc, char *argv[])
{
  printf("Main's pid: %d\n", getpid());
  int answer = 0;
  do {
    printf("The root is: %d\n", root);
    answer = show_menu_get_answer();
    switch (answer) {
      case 1:
        main_create();
        break;
      case 2:
        main_search();
        break;
      case 3:
        main_delete();
        break;
      default:
        break;
    }
  } while(answer != 5);

  return 0;
}

void get_value(char* value){
  printf("Value: ");
  scanf("%s", value);
}

void main_create(){
  char value[12];
  get_value(value);
  if(root == 0){
    fork_node(value, &root);
    root_value = atoi(value);
    msgq_id = create_msgq(&root);
  }
  else {
    char* message = malloc(sizeof(char*));
    strcpy(message, "create");
    strcat(message, value);
    send_message(message, &root, msgq_id);
    free(message);
  }
}

void main_search(){
  char value[12];
  get_value(value);

  if(root == 0){
    printf("Doesn't exist!\n");
  }else {
    char* message = malloc(sizeof(char*));
    strcpy(message, "search");
    strcat(message, value);
    send_message(message, &root, msgq_id);
    free(message); 
  }
}

void main_delete(){
  char value[12];
  get_value(value);

  if(root == 0){
    printf("Doesn't exist!\n");
  }else {
    char* message = malloc(sizeof(char*));
    strcpy(message, "delete");
    strcat(message, value);
    send_message(message, &root, msgq_id);
    sleep(2);
    if(root_value == atoi(value)){
      delete_msgq(msgq_id);
      root = 0;
    }
    free(message); 
  }
}