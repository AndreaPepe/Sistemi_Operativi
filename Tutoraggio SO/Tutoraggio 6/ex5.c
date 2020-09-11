#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define N 5
#define SIZE 1024
typedef struct _container{
	unsigned int available;
	char mex[SIZE];
}container;


container array[N];

void* thread_f(void* arg){
	int j = *((int*) &arg);
	while(array[j].available==0);
	printf("[Thread %d] I received: %s\n",j,array[j].mex);
	if(j<N-1){
		printf("[Thread %d] Insert a message for thread %d: \n",j,j+1);
		scanf("%[^\n]",array[j+1].mex);
		while(getchar()!=(int)'\n');

		array[j].available=0;
		array[j+1].available=1;
	}
	pthread_exit((void*) 0);
}



int main(int argc, char **argv){
	
	pthread_t threads[N];
	int i;
	for(i=0;i<N;i++){
		array[i].available=0;
	//	array[i].mex=NULL;
	}
	
	printf("[Main thread] I'm generating %d threads\n\n",N);

	for(i=0;i<N;i++){
		if(pthread_create(&threads[i],NULL,thread_f, *((void**)&i))){
			printf("Error occurred creating thread!\n");
			exit(-1);
		}

	}
	printf("[Main thread] Insert a message for thread 0: ");
	scanf("%[^\n]",array[0].mex);
	while(getchar()!=(int)'\n');
	array[0].available=1;
                
	for(i=0;i<N;i++)
		pthread_join(threads[i],NULL);

	printf("[Main thread] Job done, bye bye\n");
	return 0;

}
