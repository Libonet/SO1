#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFSIZE 2048

int main(){
    char* buff;
    int bcap = BUFFSIZE;
    buff = malloc(bcap);

    int pdes[2];
    if (pipe(pdes)){
        perror("Pipe failure");
        exit(-1);
    }
    
    int pid = fork();
    if (pid == 0){
        close(pdes[1]);
        read(pdes[0], buff, bcap);
        close(pdes[0]);
        printf("received: %s", buff);
    }
    else{
        close(pdes[0]);
        write(pdes[1], "Hola mundo\n", 12);
        close(pdes[1]);
    }

    sleep(5);
    free(buff);
    return 0;
}