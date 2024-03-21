#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    sleep(2);
    execl("./malloc", "./malloc", NULL);
    printf("Algo malió sal\n");

    return 0;
}