#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <fcntl.h>


#define SIZE 4096

int produttore(void* addr, int sem_ds){

	struct sembuf oper;

	oper.sem_num=1;
	oper.sem_op=-1;
	oper.sem_flg=0;

	semop(sem_ds,&oper,1);

	printf("[PRODUCER] Insert a message: ");
	scanf("%s",(char*)addr);

	oper.sem_num=0;
	oper.sem_op=1;
	oper.sem_flg=0;

	semop(sem_ds,&oper,1);

	return 1;

}


int main(int argc, char** argv){
	key_t m_key = 1000;
	key_t s_key = 3;
	int sem_ds, shm_ds;
	void *shm_addr;

	shm_ds=shmget(m_key,SIZE,O_CREAT|0666);

	shm_addr=shmat(shm_ds,NULL,0);

	sem_ds = semget(s_key,2, IPC_CREAT|0666);


	int ret;
	do{
		ret=produttore (shm_addr, sem_ds);
	}while(ret);


	shmdt(shm_addr);
	exit(0);

}

