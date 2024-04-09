#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void seek_write(int fd, int x, char v){
  lseek(fd, x, SEEK_SET);
  write(fd, &v, 1);
}

char seek_read(int fd, int x){
  char buf[2];
  buf[1] = '\0';

  lseek(fd, x, SEEK_SET);
  read(fd, buf, 1);

  return buf[0];
}

int main(){
  int fd = open("./racist.txt", O_RDWR);
  char c;

  pid_t pid = fork();
  for(int i=0; i<5; i++){
    switch(pid){
      case 0:
	c = seek_read(fd, i);
	printf("Child leyo: %c\n", c);
	seek_write(fd, i, 'c');
        break;

      default:
        c = seek_read(fd, i);
        printf("Parent leyo: %c\n", c);
	seek_write(fd, i, 'p');
        break;
    }
  }

  close(fd);
  return 0;
}
