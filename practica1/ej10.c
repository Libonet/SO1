#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler_SIGUSR1(int);

int fpid;
int cpid;

int main(){
	fpid = getpid();
	int pid = fork();
	switch(pid){
		case -1:
			perror("Fallo!");
			exit(1);
		case 0:
			cpid = getpid();
			signal(SIGUSR1, handler_SIGUSR1);
			printf("Child signal set\n");
			while(1);
       		default:	
			signal(SIGUSR1, handler_SIGUSR1);
			printf("Father signal set\n");
			sleep(1);
			kill(pid, SIGUSR1);
			while(1);
	}
	return 0;
}

void handler_SIGUSR1(int signum){
	int myPid = getpid();
	if (myPid == cpid){	
		printf("Soy el hijo\n");
		sleep(1);
		kill(fpid, SIGUSR1);
	} else if(myPid == fpid){
		printf("Soy el padre\n");
		sleep(1);
		kill(cpid, SIGUSR1);
	} else{
		perror("wtf how did we get here\n");	
		exit(-1);		
	}
	/*printf("%d: Saltó sigusr1", getpid());
	switch(getpid()){
		case hpid:
			kill(fpid, SIGUSR1);
			breaK;
		case fpid:
			kill(hpid, SIGUSR1);
			break;
	}*/
}
