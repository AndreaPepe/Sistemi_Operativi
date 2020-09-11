#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define SIZE 100
#define END 1000000

long *v;
int sem_available_slot;
int sem_available_item;


void* producer(void){
	
	long data=0;
	long j=0;

	struct sembuf oper;
	printf("ready to produce\n");
retry:
	oper.sem_num = j;
	oper.sem_op = -1;
	oper.sem_flg = 0;
	
	semop(sem_available_slot,&oper,1);
	
	v[j] = data;
	oper.sem_num=j;
	oper.sem_op= 1;
	oper.sem_flg=0;

	semop(sem_available_item,&oper,1);

	j=(j+1)%SIZE;
	data++;
	
	goto retry;
}

void* consumer(void){
        
	long data=0,val;
        long j=0;

        struct sembuf oper;
	printf("ready to consume\n");
retry:
        oper.sem_num = j;
        oper.sem_op = -1;
        oper.sem_flg = 0;

        semop(sem_available_item,&oper,1);

        val=v[j];

//	printf("%ld\n",val);

	if(val!=data){
		printf("synchronization protocol broken\n");
		exit(-1);
	}

	if(val==END){
		printf("ok, last value read is %ld\n\n",val);
		exit(0);
	}

        oper.sem_num=j;
        oper.sem_op= 1;
        oper.sem_flg=0;

        semop(sem_available_slot,&oper,1);

        j=(j+1)%SIZE;
        data++;

        goto retry;
}

int main(int argc, char** argv){
	
	int prod,cons;
	int i;
	key_t key = IPC_PRIVATE;

	v=(long*)mmap(NULL,sizeof(long)*SIZE,PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,0,0);
	if (v==NULL){
		printf("mmap error\n");
		exit(-1);
	}	

	sem_available_slot=semget(key,SIZE,IPC_CREAT|0666);
	sem_available_item=semget(key,SIZE,IPC_CREAT|0666);
	
	for(i=0;i<SIZE;i++){	
		semctl(sem_available_slot,i,SETVAL,1);
	}
	
	 for(i=0;i<SIZE;i++){
                semctl(sem_available_item,i,SETVAL,0);
        }


	prod=fork();
	if(prod==0){
		producer();
	}
	cons=fork();
	if(cons==0)
		consumer();
		
	wait(NULL);

	semctl(sem_available_slot,IPC_RMID,0);
	semctl(sem_available_item,IPC_RMID,0);
	//printf("ok\n");

	exit(0);
}
