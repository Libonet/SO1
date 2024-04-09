#include <signal.h>
#include <unistd.h>
#include <stdio.h>



int main(){
  void (*signalReturnHandler)(int);
  signalReturnHandler = signal(SIGFPE, divByZeroHandler);

  return 0;
}
