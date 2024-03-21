#include <unistd.h>
#include <stdio.h>

int main(){
    pid_t id;
    int hoal = 3;
    int chual = 4; // oneupping Andres Guido Grillo
    printf("Soy el hijo????\n\n");

    id = fork();
    if (id == 0){
        // printf("child_pid: %d\n", getpid());
        printf("Soy el hijo 😂\n");
        printf("My pid: %d\n", getpid());
    }
    else{
        // printf("father_pid: %d\n", getpid());
        printf("No soy el hijo ;D\n");
        printf("My child pid: %d\n", id);
    }

    return 0;
}