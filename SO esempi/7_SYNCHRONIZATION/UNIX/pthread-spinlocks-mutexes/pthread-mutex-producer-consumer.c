#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>



#define SIZE (100000)
#define END (100000000)

#define AUDIT if(0)

pthread_mutex_t global_lock;	// unico MUTEX (binary semaphore) globale

long v[SIZE] = {[0 ... (SIZE-1)] -1};	// sezione critica	

long counter = 0;

void * producer(void* dummy){

	long data = 0;
	long my_index = 0;

	printf("ready to produce\n");

retry:
	pthread_mutex_lock(&global_lock);	// lock del mutex

	if(counter< SIZE){	
		v[my_index] = data;

		my_index = (my_index+1)%SIZE;
		data++;
		counter++;
	}

	pthread_mutex_unlock(&global_lock);	// unlock del mutex

	goto retry;


}

void * consumer(void* dummy){

	long data = 0;
	long my_index = 0;
	long value;

	printf("ready to consume\n");

retry:
	pthread_mutex_lock(&global_lock);	// lock del mutex
	if(counter>0){
		value = v[my_index];

		AUDIT
		printf("consumer got value %d\n",value);

		if(value != data){
			printf("consumer: synch protocol broken at expected value: %d - real is %d!!\n",data+1,value);
			exit(-1);
		};

		if (value == END){
			printf("ending condition met - last read value is %d\n",value);
			exit(0);
		}


		my_index = (my_index+1)%SIZE;
		
		data++;
		counter--;
	}
	pthread_mutex_unlock(&global_lock);	// unlock del mutex

	goto retry;

}

int main(int argc, char** argv){

	pthread_t prod, cons;

	pthread_mutex_init(&global_lock,NULL);	// inizializzo il mutex per i threads figli

	pthread_create(&cons,NULL,consumer,NULL);
	pthread_create(&prod,NULL,producer,NULL);

	pause();

}

