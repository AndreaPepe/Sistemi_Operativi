#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


pthread_spinlock_t global_lock;

#define SIZE (100000)
#define END (100000000)

#define AUDIT if(0)

long v[SIZE] = {[0 ... (SIZE-1)] -1};		// DATI riguardanti la SEZIONE CRITICA

long counter = 0;

void * producer(void* dummy){

	long data = 0;
	long my_index = 0;

	printf("ready to produce\n");

retry:
	pthread_spin_lock(&global_lock);	// cerca di poter acquisire il lock
	
	if(counter < SIZE){
		v[my_index] = data;
		my_index = (my_index+1)%SIZE;
		data++;
		counter++;

	}

	pthread_spin_unlock(&global_lock);	// rilascia il lock

	goto retry;


}

void * consumer(void* dummy){

	long data = 0;
	long my_index = 0;
	long value;

	printf("ready to consume\n");

retry:
	pthread_spin_lock(&global_lock);	// tries to acquire the lock

	if(counter > 0){
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

	pthread_spin_unlock(&global_lock);	// releases the lock

	goto retry;

}

int main(int argc, char** argv){

	pthread_t prod, cons;

	pthread_spin_init(&global_lock,0);	// inizializzazione del lock

	pthread_create(&cons,NULL,consumer,NULL);
	pthread_create(&prod,NULL,producer,NULL);

	pause();

}

