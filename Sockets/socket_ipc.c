#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

void doprocessing (int sock);
void socket_serv();

int main( int argc, char *argv[] ) {
  socket_serv();

  return 0;
}

void socket_serv()
{
  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n, pid;

  /* First call to socket() function */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
     perror("ERROR opening socket");
     exit(1);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = 8888;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
     perror("ERROR on binding");
     exit(1);
  }

  listen(sockfd, 5);
  clilen = sizeof(cli_addr);

  while (1) {
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
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
        close(sockfd);
        doprocessing(newsockfd);
        exit(0);
     }
     else {
        wait(NULL);
        close(newsockfd);
     }

  }
}

void doprocessing (int sock) {
  while (1) {
   int n;
   char buffer[256];
   bzero(buffer,256);
   n = read(sock, buffer, 255);

   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);
   if(buffer[0] == '0')
    break;
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
 }
}
