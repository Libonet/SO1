#include <pthread.h>
#include <stdio.h>

int i=0;

#define _GNU_SOURCE

void* dummy(void* arg){

	printf("El argumento que tomo es: %d\n", *(int*)arg);

	pthread_exit(&i);
}


int main(){
	setbuf(stdout, NULL);

	pthread_t id;

	int argument = 5;
	pthread_create(&id, NULL, dummy, &argument);

	//pthread_yield();

	int* ptr;

	pthread_join(id, (void**)&ptr);

	printf("Valor recibido de salida del thread %ld: %i\n", id, *ptr);

	printf("main\n");
	return 0;
}
