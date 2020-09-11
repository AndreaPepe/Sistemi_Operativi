#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE (100000)

long v[SIZE] = {[0 ... (SIZE-1)] -1};		// all elements are setted to -1

long counter = 0;

void * producer(void* dummy){

	long data = 0;
	long my_index = 0;

	printf("ready to produce\n");

retry:
	while(counter < SIZE){
		v[my_index] = data;
		my_index = (my_index+1)%SIZE;	// BUFFER CIRCOLARE
		data++;
		counter++;
	
	}
	goto retry;


}

void * consumer(void* dummy){

	long data = 0;
	long my_index = 0;
	long value;

	printf("ready to consume\n");

retry:
	while(counter > 0){
		value = v[my_index];
		printf("got value %ld\n",value);
		if(value != data){
			printf("consumer: synch protocol broken at expected value: %ld - real is %ld!!\n",data,value);
			exit(-1);

		};
		data++ ;
		my_index = (my_index+1)%SIZE;
		counter--;
	
	}
	goto retry;

}

int main(int argc, char** argv){

	pthread_t prod, cons;

	pthread_create(&cons,NULL,consumer,NULL);
	sleep(1);
	pthread_create(&prod,NULL,producer,NULL);

	pause();

}

