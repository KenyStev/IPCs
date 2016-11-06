#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

void socketConfig(int * sock, struct sockaddr_in* server);
void socket_write(int* sock);

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    socketConfig(&sock, &server);

    puts("Connected\n");
    //keep communicating with server
    socket_write(&sock);

    close(sock);
    return 0;
}

void socketConfig(int * sock, struct sockaddr_in* server){
  //Create socket
  *sock = socket(AF_INET , SOCK_STREAM , 0);
  if (*sock == -1)
  {
    printf("Could not create socket");
  }
  puts("Socket created");

  (*server).sin_addr.s_addr = inet_addr("127.0.0.1");
  (*server).sin_family = AF_INET;
  (*server).sin_port = htons( 8888 );
  //Connect to remote server
  if (connect(*sock , (struct sockaddr *)server , sizeof(*server)) < 0)
  {
    perror("Connect failed. Error");
    return;
  }

}

void socket_write(int* sock){
  char message[1000] , server_reply[2000];
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
    if( recv(*sock , server_reply , 2000 , 0) < 0)
    {
      puts("recv failed");
      break;
    }

    puts("Server reply :");
    puts(server_reply);
  }
}
