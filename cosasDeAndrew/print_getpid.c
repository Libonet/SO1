#include <stdio.h>
#include <unistd.h>

int main(){
    printf("child_pid: %d\n", getpid());
    return 0;
}