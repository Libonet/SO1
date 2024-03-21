#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define BUFFSIZE 2048

int main(){
    char* buff;
    int bcap = BUFFSIZE;
    int sv[2];
    buff = malloc(bcap);
    socketpair(AF_UNIX, SOCK_DGRAM, 0, sv);

    switch (fork()){
    case -1:
        free(buff);
        perror("error uwu\n");
        exit(EXIT_FAILURE);
    case 0: // hijo
        close(sv[1]);

        read(sv[0], buff, bcap);

        printf("child received: %s", buff);

        write(sv[0], "Message received\n", 18);

        break;
    default: // padre
        close(sv[0]);

        write(sv[1], "Hola mundo\n", 12);

        read(sv[1], buff, bcap);

        printf("father received: %s", buff);

        wait(NULL);
        break;
    }

    free(buff);
    return 0;
}