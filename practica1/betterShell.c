#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <fcntl.h>

#define BSIZE 1024 // default buffer size

// struct intCounts{
//   int tokenCount;
//   int nullCount;
// };

// void remove_enter(char** buffer){
//   int i=0;
//   char c;
//   for (; (c=(*buffer)[i])!='\0'; i++) {
//     if (c=='\n'){
//       (*buffer)[i] = '\0';
//     }
//   }
// }

// // separa un string de distintas palabras y las almacena en el array "command"
// struct intCounts tokenizar(char *buffer, char* **command, int* commandSize, int* *pipeArray, int* pipeArraySize){
//   struct intCounts counts;
//   int tokenCount = 0, tokenExit=0, nullCount = 1; // falso NULL antes del primer comando
//   char *token;

//   // separamos el input en distintos tokens y los guardamos en "command"
//   token = strtok(buffer, " ");
//   while (tokenExit != 1){
//     if (token == NULL){
//       tokenExit = 1;
//     }
//     // printf("token: %s\n", token);
//     if (tokenCount == *commandSize){
//       *commandSize *= 2;
//       *command = realloc(*command, sizeof(char*)*(*commandSize));
//     }

//     // reemplazamos pipe por NULL, para terminar el comando, y almacenamos su indice
//     if (token == NULL || token[0] == '|'){ 
//       (*command)[tokenCount] = NULL;

//       if (nullCount == *pipeArraySize){
//         *pipeArraySize *= 2;
//         *pipeArray = realloc(*command, sizeof(int)*(*pipeArraySize));
//       }
//       (*pipeArray)[nullCount] = tokenCount;
//       nullCount++;
//     } else{
//       (*command)[tokenCount] = token;
//     }
//     tokenCount++;

//     token = strtok(NULL, " ");
//   }
//   counts.tokenCount = tokenCount;
//   counts.nullCount = nullCount;
//   return counts;
// }

// int builtinHandling(char** command){
//   // si se ingresa "exit" salimos del programa
//   if (strcmp(command[0], "exit")==0){
//     return 1;
//   }
//   // funciones especiales (built-in)
//   if (strcmp(command[0], "cd")==0){
//     chdir(command[1]);
//     return 2;
//   }
//   return 0;
// }

// void runCommand(char **command, int *pipeArray, int index){
//   int fd = -1;
//   // checkeamos >
//   int redirectPos = pipeArray[index+1]-2;
//   if (redirectPos>=0 && command[redirectPos][0]=='>'){
//     fd = open(command[pipeArray[index]-1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
//     dup2(fd, STDOUT_FILENO);
//   }

//   execvp(command[0], command);

//   close(fd);
//   free(pipeArray);
//   free(command[0]); // same as free(buffer)
//   free(command);

//   exit(-1); // si el comando no existe, se alcanza esta linea
// }

// int recursivePiping(char **command, int commandIndex, int *pipeArray, int nullCount){
//   int pdes[2], status;
//   char* *nextCommand;

//   if (commandIndex==nullCount){
//     return 0;
//   }

//   pipe(pdes);
//   switch (fork()){
//     case -1:
//       fprintf(stderr, "Wtf, fork failed in recursive piping\n");
//       return -1;
//     case 0:
//       close(pdes[1]);
//       dup2(pdes[0], STDIN_FILENO);
//       if (commandIndex < nullCount){
//         nextCommand = (command+pipeArray[commandIndex]+1);
//         recursivePiping(nextCommand, commandIndex+1, pipeArray, nullCount); // normalmente no volvemos de aca
//       }
//       if (builtinHandling(nextCommand)==1){ // si no hay un hijo siguiente
//         exit(0);
//       } 
//       break;
//     default:
//       close(pdes[0]);
//       dup2(pdes[1], STDOUT_FILENO);
//       int builtinReturn = builtinHandling(command);
//       if (builtinReturn==1){
//         exit(0);
//       } else if (builtinReturn == 0){
//         runCommand(command, pipeArray, commandIndex);
//       }
//       return 1; // command not found
//   }
//   return 0;
// }


// Declaracion de las funciones builtin
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

// Lista de comandos builtin
char* builtin_str[] = {
  "cd",
  "help",
  "exit"
};

// Lista de funciones builtin para llamarlas por su referencia
int (*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_help,
  &shell_exit
};

int shell_num_builtins() {
  return sizeof(builtin_str)/sizeof(char *);
}


// Implementacion de funciones Builtin.
int shell_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "shell: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      fprintf(stderr, "cd failure\n");
    }
  }
  return 1;
}

int shell_help(char **args)
{
  int i;
  printf("Mi shell :D\n");
  printf("Escribi el nombre de un programa y sus argumentos, y apreta enter\n");
  printf("Los siguientes programas son builtin:\n");

  for (i = 0; i < shell_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]); // para esto las declaramos primero
  }

  printf("Usa el comando 'man' para ver ayuda de otros programas.\n");
  return 1;
}

int shell_exit(char **args){
  return 0;
}

char* shell_get_line(){
  char *line = NULL;
  ssize_t bufsize = 0; // getline se encarga de reservar espacio
  ssize_t nbytes;

  nbytes = getline(&line, &bufsize, stdin);
  if (nbytes == -1){
    free(line);
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // recibimos EOF
    } else  {
      fprintf(stderr, "get_line\n");
      exit(EXIT_FAILURE); // error en getline
    }
  }

  line[nbytes-1] = '\0'; // pisamos el '\n'

  return line;
}

char** shell_split_line(char* line){
  int argsSize=10, tokenCount = 0;
  char *token;
  char **args;

  args = malloc(sizeof(char*)*argsSize);
  if (!args){
    fprintf(stderr, "malloc\n");
    exit(EXIT_FAILURE);
  }

  // separamos el input en distintos tokens y los guardamos en "args"
  token = strtok(line, " ");
  while (token != NULL) {
    args[tokenCount] = token;
    tokenCount++;

    if (tokenCount >= argsSize) {
      argsSize *= 2;
      args = realloc(args, argsSize * sizeof(char*));
      if (!args) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, " ");
  }
  args[tokenCount] = NULL;
  return args;
}



int shell_start_process(char** args, int* next_pipe, int* wpid){
  pid_t pid;
  int status;

  pid = fork();
  switch (pid){
  case -1:
    fprintf(stderr, "fork in shell_start_process\n");
    break;
  case 0:
    // Child process
    if (execvp(args[0], args) == -1) {
      fprintf(stderr, "execvp in shell_start_process\n");
      exit(EXIT_FAILURE);
    }
  default:
    *wpid = pid;
    // Parent process
    if (next_pipe!=NULL){
      close(next_pipe[0]);
      close(next_pipe[1]);
    }
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    break;
  }

  return 1;
}

int shell_execute(char** args, int* next_pipe, int* wpid){
  if (args[0] == NULL) {
    // Se ingreso un comando vacio
    return 1;
  }

  for (int i = 0; i < shell_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return shell_start_process(args, next_pipe, wpid);
}

typedef struct command_array{
  char** args; // palabras separadas por espacios (todos los comandos separados por NULL)
  int commandCount; // cantidad de comandos
  char*** commandArray; // puntero a cada comando
  int commandArrayCap; // tamaño reservado para commandArray
} commands;

int shell_multiple_execute(commands comms){
  int index, status;
  char** command;
  int *next_pipe = malloc(sizeof(int)*2);
  int* wpid = malloc(sizeof(int)*comms.commandCount);

  int previousPipeOutput = STDIN_FILENO;
  for (index = 0; index<comms.commandCount; index++){
    command = comms.commandArray[index];

    if (index == comms.commandCount-1){
      free(next_pipe);
      next_pipe = NULL;
    } else{
      pipe(next_pipe);
    }
    switch (fork()){
    case 0: // child process
      dup2(previousPipeOutput, STDIN_FILENO);
      if (next_pipe!=NULL){
        // me encargo del pipe
        dup2(next_pipe[1], STDOUT_FILENO);
        close(next_pipe[0]);
      }
      shell_execute(command, next_pipe, (wpid+index));
      exit(EXIT_SUCCESS);
    
    default: // parent process
      if (index!=0){
        close(previousPipeOutput);
      }
      if (next_pipe){
        close(next_pipe[1]);
        previousPipeOutput = next_pipe[0]; // guardamos la salida para el siguiente proceso hijo
      }
      break;
    }
  }

  for (index = 0; index<comms.commandCount; index++){
    do {
      waitpid(wpid[index], &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

// Separa una cadena de palabras en distintos comandos separados por |
commands shell_separate_commands(char** args){
  commands comms;
  comms.args = args;
  comms.commandCount = 0;

  comms.commandArrayCap = 10;
  comms.commandArray = malloc(comms.commandArrayCap*sizeof(char**));
  comms.commandArray[0] = NULL;

  if (args[0]==NULL){
    return comms;
  }

  if (strcmp(args[0],"|")!=0){
    comms.commandArray[0] = &args[0];
    comms.commandCount++;
  }

  char* arg;
  for (int index = 0; (arg = args[index]) != NULL; index++){
    if (strcmp(arg,"|")==0){
      args[index] = NULL;
      index++;
      if (args[index] != NULL){
        if (strcmp(args[index], "|")==0){
          fprintf(stderr, "shell: syntax error near unexpected token '|'\n");
          comms.commandCount = 0; // al salir, no se va a ejecutar nada y se libera la memoria
          return comms;
        } else{
          comms.commandArray[comms.commandCount] = (args+index);
          comms.commandCount++;
        }
      }
    }
  }
  
  return comms;
}

void shell_loop(void){
  char *line;
  char **args;
  commands comms;
  int status, wpid;

  do {
    printf("> "); // podria mostrar distintas cosas teniendo en cuenta la configuracion
    line = shell_get_line();
    args = shell_split_line(line);
    comms = shell_separate_commands(args);

    if (comms.commandCount!=0){ // Si es 0, no hay nada que ejecutar
      if (comms.commandCount==1){
        status = shell_execute(args, NULL, &wpid);
      } else{
        status = shell_multiple_execute(comms);
      }
    }

    free(line);
    free(args);
    free(comms.commandArray);
  } while (status);
}

int main(){
  // TODO: cargar configuracion (y utilizarla)

  shell_loop();

  // Limpiar antes de salir

  return 0;
}

// struct intCounts counts;
//   int *pipeArray, builtinReturn;
//   int tokenCount, nullCount, commandSize = 10, pipeArraySize = 5, wstatus=0, shellExit=0;
//   char *buffer, **command;

//   pipeArray = malloc(sizeof(int)*pipeArraySize);
//   buffer = malloc(BSIZE);
//   command = malloc(sizeof(char*)*commandSize);

//   pipeArray[0] = -1;
//   while (shellExit != 1) {
//     // tomamos el input
//     fgets(buffer, BSIZE, stdin);
//     remove_enter(&buffer);

//     counts = tokenizar(buffer, &command, &commandSize, &pipeArray, &pipeArraySize);
//     tokenCount = counts.tokenCount;
//     nullCount = counts.nullCount;

//     // si no se ingresa un comando, seguimos
//     if (tokenCount==1){ // se cuenta al NULL como token
//       continue;
//     }

//     builtinReturn = builtinHandling(command);
//     if (builtinReturn == 1){
//       shellExit = 1;
//       continue;
//     }

//     // Si tenemos un posible comando, creamos un hijo para que ejecute el comando
//     switch(fork()){
// 	    case -1:
// 	      fprintf(stderr, "Wtf, esto no debería pasar\n");
//         free(pipeArray);
//         free(command[0]); // same as free(buffer)
//         free(command);
// 	      exit(-1);
//       case 0:
//         // este proceso es una funcion recursiva
//         // pipe
//         // fork
//         // -PADRE: STDOUT -> pipe[1], -HIJO: STDIN -> pipe[0]
//         // hijo: sigue la recursion
//         if (nullCount >= 3){ // hay al menos 1 pipe en el comando
//           if (recursivePiping(command, 0, pipeArray, nullCount)==-1){ // fork failed
//             free(pipeArray);
//             free(command[0]); // same as free(buffer)
//             free(command);
//             exit(-1);
//           }
//         } else{
//           builtinReturn = builtinHandling(command);
//           if (builtinReturn==0){
//             runCommand(command, pipeArray, 0);
//           }
//         }
//       default:
//         wait(&wstatus);
//         // printf("exit status: %d\n", wstatus);
//     }
//   }

//   free(pipeArray);
//   free(command[0]); // same as free(buffer)
//   free(command);