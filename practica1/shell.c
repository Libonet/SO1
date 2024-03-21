#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#define BSIZE 1024

void removeEnter(char** buffer){
  int i=0;
  char c;
  for (; (c=(*buffer)[i]); i++) {
    if (c=='\n'){
      (*buffer)[i] = '\0';
    }
  }
}

int main(){
  int tokenCount = 0, commandSize = 20, wstatus, exitSignal=0;
  char *buffer, *token, **command;
  buffer = malloc(BSIZE);
  command = malloc(sizeof(char*)*20);

  while (1) {
    tokenCount = 0;
    fgets(buffer, BSIZE, stdin);
    removeEnter(&buffer);
    token = strtok(buffer, " ");
    while (exitSignal != 1){
      if (token == NULL){
        exitSignal = 1;
      }
      // printf("token: %s\n", token);
      if (tokenCount == commandSize){
        commandSize *= 2;
        command = realloc(command, commandSize);
      }
      command[tokenCount] = token;
      tokenCount++;

      token = strtok(NULL, " ");
    }
    if (strcmp(command[0][0], "exit")==0){
      break;
    }

    switch(fork()){
      case -1:
        // handle error
        break;
      case 0: // hijo
        // if (strcmp(command[tokenCount-3], ">")==0){}
        execvp(command[0], command);
        break;
      default: // padre
        wait(&wstatus);
        break;
    }
    // for (size_t i = 0; i < tokenCount-1; i++) {
    //   free(command[i]);
    // }
  }

  free(command);
  free(buffer);

  return 0;
}
