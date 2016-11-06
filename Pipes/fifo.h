#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_BUF 1024

void closeFifoRM(int fd){
    //printf("Closing FIFO RD fd: %d; pid: %d;\n", fd, getpid());
	close(fd);
}

void closeFifoWM(int fd, char ** myfifo){
    //printf("Closing FIFO WM fd: %d; pid name: %s;\n", fd, *myfifo);
	close(fd);
    unlink(*myfifo);
}

void openFIFO(char ** myfifo, int *fd){
    int parentPID = getpid();
    //char buffer[4];
    sprintf(*myfifo, "%d", parentPID);
    //printf("FIFO (R): %s\n", buffer);
    //*myfifo = buffer;

    /* open, read, and display the message from the FIFO */
    *fd = open(*myfifo, O_RDONLY);
    //printf("OpenFifo: %s\n", *myfifo);
}

void createFIFO(int *fd, char ** myfifo, int pid){
	//char buffer[33];

	sprintf(*myfifo, "%d", pid);
    //*myfifo = buffer;

    mkfifo(*myfifo, 0666);

    *fd = open(*myfifo, O_WRONLY);
    //printf("CreateFifo: %s\n", *myfifo);
}


void sendMessage(int fd, char* message){
    //printf("Sending message: %s\n", message);
    write(fd, message, strlen(message) + 1);
}

void receiveMessage(int fd, char * option, char* NodeValue, char * message){
	char buf[MAX_BUF];
	
    read(fd, buf, MAX_BUF);
    //if(buf != "*")
    //printf("Received: %s.\n", buf);

    char t_option[7];
    char t_nodeValue[5];

    for (int i = 0; i < 6; ++i)
    {	
    	t_option[i] = buf[i];
    	t_nodeValue[i] = buf[i+7];
    }
    strcpy(option, t_option);
    strcpy(NodeValue, t_nodeValue);
    strcpy(message, buf);
}
