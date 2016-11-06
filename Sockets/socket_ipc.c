#include "node_utils.c"
//attributes
pnode_t node_attr;
pid_t root;

int main(int argc, char *argv[]){
  node_attr.val = atoi(argv[1]);
  printf("My value is %d\n", node_attr.val);
  socket_serv();
  pthread_t pth;
  // pthread_create(&pth, NULL, (void*)socket_serv, NULL);
  // socket_serv();
  return 0;
}
