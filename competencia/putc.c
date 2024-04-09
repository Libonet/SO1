#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void write_to_stdout(char* str){
  int c;

  for(int i=0; (c=str[i])!='\0'; i++){
    putc(c, stdout);
  }
}

int main(){
  setbuf(stdout, NULL);
  pid_t pid = fork();
  for(int i=0; i<5; i++){
    switch(pid){
      case 0:
        write_to_stdout("Child: Hola Mundo!\n");
        break;

      default:
        write_to_stdout("Parent: Hola Mundo!\n"); 
        break;
    }
  }

  return 0;
}
