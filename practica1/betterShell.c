#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <fcntl.h>

#define BSIZE 1024

struct intCounts{
  int tokenCount;
  int nullCount;
};

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
struct intCounts tokenizar(char *buffer, char* **command, int* commandSize, int* *pipeArray, int* pipeArraySize){
  struct intCounts counts;
  int tokenCount = 0, tokenExit=0, nullCount = 1; // falso NULL antes del primer comando
  char *token;

  // separamos el input en distintos tokens y los guardamos en "command"
  token = strtok(buffer, " ");
  while (tokenExit != 1){
    if (token == NULL){
      tokenExit = 1;
    }
    // printf("token: %s\n", token);
    if (tokenCount == *commandSize){
      *commandSize *= 2;
      *command = realloc(*command, sizeof(char*)*(*commandSize));
    }

    // reemplazamos pipe por NULL, para terminar el comando, y almacenamos su indice
    if (token == NULL || token[0] == '|'){ 
      (*command)[tokenCount] = NULL;

      if (nullCount == *pipeArraySize){
        *pipeArraySize *= 2;
        *pipeArray = realloc(*command, sizeof(int)*(*pipeArraySize));
      }
      (*pipeArray)[nullCount] = tokenCount;
      nullCount++;
    } else{
      (*command)[tokenCount] = token;
    }
    tokenCount++;

    token = strtok(NULL, " ");
  }
  counts.tokenCount = tokenCount;
  counts.nullCount = nullCount;
  return counts;
}

int builtinHandling(char** command){
  // si se ingresa "exit" salimos del programa
  if (strcmp(command[0], "exit")==0){
    return 1;
  }
  // funciones especiales (built-in)
  if (strcmp(command[0], "cd")==0){
    chdir(command[1]);
    return 2;
  }
  return 0;
}

void runCommand(char **command, int *pipeArray, int index){
  int fd = -1;
  // checkeamos >
  int redirectPos = pipeArray[index+1]-2;
  if (redirectPos>=0 && command[redirectPos][0]=='>'){
    fd = open(command[pipeArray[index]-1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    dup2(fd, STDOUT_FILENO);
  }

  execvp(command[0], command);

  close(fd);
  free(pipeArray);
  free(command[0]); // same as free(buffer)
  free(command);

  exit(-1); // si el comando no existe, se alcanza esta linea
}

int recursivePiping(char **command, int commandIndex, int *pipeArray, int nullCount){
  int pdes[2], status;
  char* *nextCommand;

  if (commandIndex==nullCount){
    return 0;
  }

  pipe(pdes);
  switch (fork()){
    case -1:
      perror("Wtf, fork failed in recursive piping\n");
      return -1;
    case 0:
      close(pdes[1]);
      dup2(pdes[0], STDIN_FILENO);
      if (commandIndex < nullCount){
        nextCommand = (command+pipeArray[commandIndex]+1);
        recursivePiping(nextCommand, commandIndex+1, pipeArray, nullCount); // normalmente no volvemos de aca
      }
      if (builtinHandling(nextCommand)==1){ // si no hay un hijo siguiente
        exit(0);
      } 
      break;
    default:
      close(pdes[0]);
      dup2(pdes[1], STDOUT_FILENO);
      int builtinReturn = builtinHandling(command);
      if (builtinReturn==1){
        exit(0);
      } else if (builtinReturn == 0){
        runCommand(command, pipeArray, commandIndex);
      }
      return 1; // command not found
  }
  return 0;
}

int main(){
  struct intCounts counts;
  int *pipeArray, builtinReturn;
  int tokenCount, nullCount, commandSize = 10, pipeArraySize = 5, wstatus=0, shellExit=0;
  char *buffer, **command;

  pipeArray = malloc(sizeof(int)*pipeArraySize);
  buffer = malloc(BSIZE);
  command = malloc(sizeof(char*)*commandSize);

  pipeArray[0] = -1;
  while (shellExit != 1) {
    // tomamos el input
    fgets(buffer, BSIZE, stdin);
    removeEnter(&buffer);

    counts = tokenizar(buffer, &command, &commandSize, &pipeArray, &pipeArraySize);
    tokenCount = counts.tokenCount;
    nullCount = counts.nullCount;

    // si no se ingresa un comando, seguimos
    if (tokenCount==1){ // se cuenta al NULL como token
      continue;
    }

    builtinReturn = builtinHandling(command);
    if (builtinReturn == 1){
      shellExit = 1;
      continue;
    }

    // Si tenemos un posible comando, creamos un hijo para que ejecute el comando
    switch(fork()){
	    case -1:
	      perror("Wtf, esto no debería pasar\n");
        free(pipeArray);
        free(command[0]); // same as free(buffer)
        free(command);
	      exit(-1);
      case 0:
        // este proceso es una funcion recursiva
        // pipe
        // fork
        // -PADRE: STDOUT -> pipe[1], -HIJO: STDIN -> pipe[0]
        // hijo: sigue la recursion
        if (nullCount >= 3){ // hay al menos 1 pipe en el comando
          if (recursivePiping(command, 0, pipeArray, nullCount)==-1){ // fork failed
            free(pipeArray);
            free(command[0]); // same as free(buffer)
            free(command);
            exit(-1);
          }
        } else{
          builtinReturn = builtinHandling(command);
          if (builtinReturn==0){
            runCommand(command, pipeArray, 0);
          }
        }
      default:
        wait(&wstatus);
        // printf("exit status: %d\n", wstatus);
    }
  }

  free(pipeArray);
  free(command[0]); // same as free(buffer)
  free(command);

  return 0;
}
