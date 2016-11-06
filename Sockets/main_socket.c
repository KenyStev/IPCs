#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<pthread.h>
#include "node_utils.c"

int show_menu_get_answer(){
    printf("1. Create process.\n2. Search process.\n3. Delete process.\nAnswer: ");
    int answer;
    scanf("%d", &answer);
    return answer;
}

int main(int argc, char const *argv[])
{
  pid_t root = 0;
  pthread_t thread;
  char value[10];
  printf("%d\n", getpid());
  int answer = 0;
  do {
    printf("The root is: %d\n", root);
    answer = show_menu_get_answer();
    switch (answer) {
      case 1:
        printf("Value: ");
        scanf("%s", &value);
        if(root == 0){
          createNode(value, &root);
          sleep(1);
          printf("Node created!\n");
          pthread_create(&thread, NULL, socket_client, (void*)&root);
          //socket_client(root);
        }
        else{

        }
        break;
      case 2:
        break;
      case 3:
        break;
      default:
        break;
    }
  } while(answer != 5);

  return 0;
}
