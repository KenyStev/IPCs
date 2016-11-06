#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "node_utils.c"

int show_menu_get_answer()
{
    printf("1. Create process.\n2. Search process.\n3. Delete process.\nAnswer: ");
    int answer;
    scanf("%d", &answer);
    return answer;
}

int create_node(int val)
{
/*  if(root == 0)
  {
    root = fork();
    // printf("%d\n", root);
    if(root == 0)
    {
      char** char_val = malloc(sizeof(char*));
      sprintf(*char_val, "%d", val);
      printf("Duuuude! %d\n", root);
      execvp("./single_server", char_val);
    }else if(root < 0)
    {
      perror("Couldn't make it!");
      exit(1);
    }else
    {
      printf("I have a son whose pid is: %d and mine is %d\n", root, getpid());
      int status = 0;
      wait(&status);
      printf("Just waiting: %d\n", WEXITSTATUS(status));
    }
  }*/
}


int main(int argc, char const *argv[])
{
  pid_t root=0;
  char value[10];
  printf("%d\n", getpid());
  switch (show_menu_get_answer()) {
    case 1:
      printf("Value: ");
      scanf("%s", &value);
      if(root == 0)
        createNode(value, &root);
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

  return 0;
}
