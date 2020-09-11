#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int glbl_flag = 0;

void* thread_function(void){
	printf("[Child thread] I'm exiting,bye!\n");
	sleep(2);
	glbl_flag=1;
}



int main (int argc , char **argv){
	pthread_t tid;

	if(pthread_create(&tid, NULL, (void*)thread_function, NULL) !=0){
		printf("pthread_create() error occurred!\n");
		exit(-1);
	}

	while(glbl_flag==0); 	// synchronize with global variable

	printf("[Main thread] I'm exiting, bye!\n");
	return 0;
}
