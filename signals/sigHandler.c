#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handleSignals(int sigCode){
  switch (sigCode) {
    case SIGQUIT:
      printf("SIGQUIT?????\n");
      break;
    case SIGINT:
      printf("SIGINT????\n");
      signal(SIGINT, SIG_DFL);
      break;
    default:
      printf("Received signal number %d\n", sigCode);
  }
}

int main(){
  void (*sigHandlerReturn)(int);
  sigHandlerReturn = signal(SIGQUIT, handleSignals);

  if (sigHandlerReturn == SIG_ERR) {
    perror("Signal Error: ");
    return 1;
  }

  sigHandlerReturn = signal(SIGINT, handleSignals);

  if (sigHandlerReturn == SIG_ERR) {
    perror("Signal Error: ");
    return 1;
  }

  while (1) {
    printf("Ingrese kill %d o presione ctrl+C para salir (2 veces)\n", getpid());
    sleep(10);
  }

  return 0;
}
