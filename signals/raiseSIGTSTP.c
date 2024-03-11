#include <stdio.h>
#include <stdlib.h>
#include <signal.h>



int main(){
  void (*sigHandlerReturn)(int);
  sigHandlerReturn = signal(SIGTSTP, SIG_IGN);
  if (sigHandlerReturn == SIG_ERR) {
    perror("Signal Error: ");
    return 1;
  }

  printf("Testing SIGTSTP\n");
  raise(SIGTSTP);
  printf("Signal SIGTSTP is ignored\n");

  return 0;
}
