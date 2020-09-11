#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/mman.h>

#define N 3
#define W 10
#define SIZE 4096


key_t shm_key = 10;
key_t sem_key = 50;
int sem_ds;
int shm_ds;


int main(int argc, char **argv){

	int ret, i;
	pid_t pid;
	struct sembuf oper;
	void* shm_addr;

	shm_ds=shmget(shm_key,SIZE, IPC_CREAT|0666);
	if(shm_ds==-1)
		exit(-1);

	sem_ds=semget(sem_key,3,IPC_CREAT|0666);
	if(sem_ds ==-1)
		exit(-1);

	ret=semctl(sem_ds,0,SETVAL,N);
	if(ret==-1)
		exit(-1);

	ret=semctl(sem_ds,1,SETVAL,0);
	if(ret==-1)
		exit(-1);

	ret=semctl(sem_ds,2,SETVAL,0);
        if(ret==-1)
                exit(-1);


	for(i=0;i<N;i++){
		pid=fork();
		if(pid==-1)
			exit(-1);
		if(pid==0)
			lettore();
	}

	shm_addr=shmat(shm_ds,NULL,)
	while(i<W){
		oper.sem_num = 0;
		oper.sem_op = -N;
		oper.sem_flg = 0;

		ret=semop(sem_ds,&oper,1);
		if (ret==-1)
			exit(-1);

		printf("Insert a message")


	
	}

}
