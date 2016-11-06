#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    int fd;
    char * myfifo = "/tmp/myfifo";

    /* create the FIFO (named pipe) with Read and Write permission (0666) */
    mkfifo(myfifo, 0666);
    printf("Waiting while other process read the message \n");

    /* write "Hi" to the FIFO */
    fd = open(myfifo, O_WRONLY);

    write(fd, "Hi", sizeof("Hi"));
    write(fd, "*", sizeof("*"));

    close(fd);

    /* remove the FIFO */
    unlink(myfifo);

    printf("Writer finish\n");

    return 0;
}