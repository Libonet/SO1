#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#define BSIZE 1024

void removeEnter(char** buffer){
  int i=0;
  char c;
  for (; (c=(*buffer)[i])!='\0'; i++) {
    if (c=='\n'){
      (*buffer)[i] = '\0';
    }
  }
}

// separa un string de distintas palabras y las almacena en el array "command"
int tokenizar(char *buffer, char* **command, int* commandSize){
  int tokenCount = 0, tokenExit=0;
  char *token;

  // separamos el input en distintos tokens y los guardamos en "command"
  token = strtok(buffer, " ");
  while (tokenExit != 1){
    if (token == NULL){
      tokenExit = 1;
    }
    // printf("token: %s\n", token);
    if (tokenCount == *commandSize){
      printf("realloc\n");
      *commandSize *= 2;
      *command = realloc(*command, sizeof(char*)*(*commandSize));
    }
    (*command)[tokenCount] = token;
    tokenCount++;

    token = strtok(NULL, " ");
  }
  return tokenCount;
}

int main(){
  int tokenCount, commandSize = 10, wstatus=0, shellExit=0;
  char *buffer, **command;
  buffer = malloc(BSIZE);
  command = malloc(sizeof(char*)*commandSize);

  while (shellExit != 1) {
    // tomamos el input
    fgets(buffer, BSIZE, stdin);
    removeEnter(&buffer);

    tokenCount = tokenizar(buffer, &command, &commandSize);

    // si no se ingresa un comando, seguimos
    if (tokenCount==1){ // se cuenta al NULL como token
      continue;
    }
    // si se ingresa "exit" salimos del programa
    if (strcmp(command[0], "exit")==0){
      shellExit = 1;
      continue;
    }
    // funciones especiales (built-in)
    if (strcmp(command[0], "cd")==0){
      chdir(command[1]);
      continue;
    }

    // Si tenemos un posible comando, creamos un hijo para que ejecute el comando
    // printf("command: ");
    // for (size_t i = 0; i < tokenCount-1; i++) {
    //   printf("%s ", command[i]);
    // }
    // putchar('\n');
    int pid = fork();
    if (pid == 0){
      execvp(command[0], command);
      exit(-1); // si el comando no existe, se alcanza esta linea
    }
    else{
      wait(&wstatus);
      // printf("exit status: %d\n", wstatus);
    }
  }

  free(command);
  free(buffer);

  return 0;
}
