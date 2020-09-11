#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void* thread_1(void *arg){
	char *buff;
	printf("[Thread 1] Insert a message: ");
	fflush(stdout);
	scanf("%m[^\n]",&buff);

	pthread_exit((void*)buff);
}



void* thread_2(void *arg){

	printf("[Thread 2] I received this message: %s\n", (char*) arg);

	pthread_exit((void*) 0);
}

int main (int argc , char **argv){

	pthread_t tid[2];
	void *buff;

	printf("Main thread: creating thread 1\n");

	if(pthread_create(&tid[0],NULL,thread_1,NULL)){
		printf("pthread_create() error occurred!\n");
		exit(-1);
	}
	pthread_join(tid[0],&buff);	// scrive in buff il valore di ritorno
	
 	printf("Main thread: creating thread 2\n");
       	fflush(stdout);

	if(pthread_create(&tid[1],NULL,thread_2,buff)){
                printf("pthread_create() error occurred!\n");
                exit(-1);
        }
	pthread_join(tid[1],NULL);

	printf("Main thread: job done, bye bye!\n");
	return 0;
	

}
