#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void sigQuitHandler(int sigNum){
    signal(SIGQUIT, sigQuitHandler);
    printf("Hijo recibe sigquit\n");
    exit(0);
}

int main(){
    int pid, fatherPID;
    setbuf(stdout, NULL);

    fatherPID = getpid();

    pid = fork();
    if (pid==0){
        struct sigaction sa;
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = ; // TODO usar sigaction en vez de signal
        printf("waiting for signals\n");
        kill(fatherPID, SIGCONT);
        while (1){}
    }
    else{
        kill(fatherPID, SIGSTOP);
        printf("sending SIGQUIT\n");
        kill(pid, SIGQUIT);

        wait(NULL);
    }

    return 0;
}