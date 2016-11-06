#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h> //for threading , link with lpthread
#include <signal.h>
#define VAL_SIZE 5

typedef struct process_node {
  int val;
  pid_t left_son;
  pid_t right_son;
} pnode_t;

int root;
//server funcs
void server_connections(int* sockfd, struct sockaddr_in* cli_addr);
int server_io (int sock);
//client funcs
void socketConfig(int * sock, struct sockaddr_in* server, int portno);
void socket_write(int* sock);
//rest
sig_atomic_t child_exit_status;
struct sigaction sigchld_action;
void clean_up_child_process (int signal_number){
  /* Clean up the child process.  */
  int status;
  wait (&status);
  child_exit_status = status;
}

void waitAsync(){
  /* Handle SIGCHLD by calling clean_up_child_process.  */
  memset (&sigchld_action, 0, sizeof (sigchld_action));
  sigchld_action.sa_handler = &clean_up_child_process;
  sigaction (SIGCHLD, &sigchld_action, NULL);
}

void createNode(char * val, pid_t *rt){
  pid_t pid;
  pid = fork();
  if(pid == 0){
    printf("Yo soy hijo\n");
    char* char_val[3];
    char_val[0]="./single_server";
    char_val[1]=val;
    char_val[2]=0;
    execvp("./single_server", char_val);
  }else{
    printf("Yo soy el padre\n");
    *rt = pid;
  }
}

//Server section
void socket_serv(){
  int sockfd, portno;
  char buffer[20];
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
     perror("ERROR opening socket");
     exit(1);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = getpid();

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
     perror("ERROR on binding");
     exit(1);
  }

  listen(sockfd, 5);
  server_connections(&sockfd, &cli_addr);
}
void server_connections(int* sockfd, struct sockaddr_in* cli_addr){
  int clilen = sizeof(*cli_addr);
  int newsockfd, pid;
  while (1) {
    newsockfd = accept(*sockfd, (struct sockaddr *) cli_addr, &clilen);
    printf("Accepted connection with %d\n", newsockfd);
    if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
    }

    pid = fork();

    if (pid < 0) {
      perror("ERROR on fork");
      exit(1);
    }

    if (pid == 0) {
      /* This is the client process */
      printf("The fork %d\n", pid);
      int exit_code = server_io(newsockfd);
      if(exit_code == 1)
        break;
      exit(0);
    }
    else {
    }
  }
  int status = 0;
  wait(&status);
  printf("Status: %d\n", WEXITSTATUS(status));
  close(newsockfd);
}
int server_io (int sock) {
  // int root;
  while (1) {
    int bytes_read;
    char buffer[20];
    bzero(buffer, 20);
    bytes_read = read(sock, buffer, 19);

    if (bytes_read < 0) {
      perror("ERROR reading from socket");
      exit(1);
    }

    printf("Message received: %s\n", buffer);
    bytes_read = write(sock, "Recv", 4);

    if (bytes_read < 0) {
      perror("ERROR writing to socket");
      exit(1);
    }

    switch (buffer[0]) {
      case 'c'://create_node
      {
        char value[VAL_SIZE];
        bzero(value, VAL_SIZE);
        int i;
        for(i = 0; i < bytes_read - 1; i++){
          value[i] = buffer[i+1];
        }
        createNode(value, &root);
      }
        break;
      case 's'://search_node
        //search_node(value, &root);
        break;
      case 'd'://delete_node
        //delete_node(value, &root);
        break;
      case '0'://exit
        return 1;
    }
  }

  return 1;
}
//Client section
void socket_client(int port_number){
  int sock;
  struct sockaddr_in server;
  socketConfig(&sock, &server, port_number);

  puts("Connected\n");
  //keep communicating with server
  socket_write(&sock);

  close(sock);
}
void socketConfig(int * sock, struct sockaddr_in* server, int portno){
  //Create socket
  printf("Port no: %d\n", portno);
  *sock = socket(AF_INET , SOCK_STREAM , 0);
  if (*sock == -1)
  {
    printf("Could not create socket");
  }
  puts("Socket created");
  //client connects with father
  (*server).sin_addr.s_addr = inet_addr("127.0.0.1");
  (*server).sin_family = AF_INET;
  (*server).sin_port = htons(portno);

  if (connect(*sock , (struct sockaddr *)server , sizeof(*server)) < 0)
  {
    perror("Connect failed. Error");
    return;
  }
}
void socket_write(int* sock){
  char message[20] , server_reply[20];
  while(1)
  {
    printf("Enter message : ");
    scanf("%s" , message);

    //Send some data
    if( send(*sock , message , strlen(message) , 0) < 0)
    {
      perror("Send failed.");
      return;
    }

    //Receive a reply from the server
    if( recv(*sock , server_reply , 10 , 0) < 0)
    {
      puts("recv failed");
      break;
    }

    puts("Server reply :");
    puts(server_reply);
  }
}
