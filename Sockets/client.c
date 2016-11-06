#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

void socket_client();
void socketConfig(int * sock, struct sockaddr_in* server);
void socket_write(int* sock);

int main(int argc , char *argv[])
{
  socket_client();
  return 0;
}

void socket_client(){
  int sock;
  struct sockaddr_in server;
  socketConfig(&sock, &server);

  puts("Connected\n");
  //keep communicating with server
  socket_write(&sock);

  close(sock);
}
void socketConfig(int * sock, struct sockaddr_in* server){
  //Create socket
  *sock = socket(AF_INET , SOCK_STREAM , 0);
  if (*sock == -1)
  {
    printf("Could not create socket");
  }
  puts("Socket created");
  int portno = getppid();//client connects with father
  (*server).sin_addr.s_addr = inet_addr("127.0.0.1");
  (*server).sin_family = AF_INET;
  (*server).sin_port = htons( portno );

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
