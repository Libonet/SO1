#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char** argv){
    if (argc != 3){
        return -1;
    }

    int pid, salida=0, wstatus;

    while (1)
    {
        pid = fork();
        if (pid == 0){
            execl(argv[1], argv[1], NULL);
        }
        wait(&wstatus);
        sleep(atoi(argv[2]));
    }
    

    return 0;
}