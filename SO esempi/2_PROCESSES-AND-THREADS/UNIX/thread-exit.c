#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<unistd.h>


void* child_thread(void*p){

	again:
	sleep(1);
	printf("I'm alive\n");
	goto again;
}

int main(int argc, char** argv){

   pthread_t tid;

   pthread_create(&tid,NULL,child_thread,NULL);

   syscall(60,0);	// 60 è il codice di syscall che identifica la exit() per singolo THREAD!; 0 è il codice di terminazione

}

/****************************************** Il main thread TERMINA, il child thread NO!!! **************************************************/
