#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define SIZE 1024

int main(){
    int fd, num_read;
    char buffer[SIZE+1];
    char path[] = "pepe.txt";

    fd = open(path, O_RDONLY, SIZE);

    num_read = read(fd, buffer, SIZE);
    buffer[num_read] = '\0';

    printf("\n\n%s\n\n\n", buffer);

    return 0;
}