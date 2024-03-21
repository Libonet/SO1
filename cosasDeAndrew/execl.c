#include <unistd.h>
#include <stdio.h>

int main(){
    printf("execl_pid: %d\n", getpid());
    execl("./hola","hola", NULL);
    return 0;
}