#include <unistd.h>
#include <stdio.h>

int main(){
    printf("padre_pid: %d\n", getpid());
    pid_t pid1 = fork();
    pid_t pid2 = fork();
    printf("pid1: %d pid2: %d\n", pid1, pid2);
    return 0;
}