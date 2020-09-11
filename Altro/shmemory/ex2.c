#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <fcntl.h>

#define SIZE 4096


void produttore(key_t key){
	
	void *message;
	int shm_ds;
	shm_ds=shmget(key, SIZE,IPC_CREAT|0666);

	message=shmat(shm_ds,NULL,0);

	printf("[Child process] Please insert a message: ");
	fflush(stdout);
	scanf("%[^\n]",(char*)message);

	exit(0);

}


int main(int argc, char** argv){

	key_t key = 32;
	int shm_ds;
	pid_t pid;
	void* message;

	//shm_ds=shmget(key,SIZE,O_CREAT|0666);
	
	pid=fork();
	if(pid==-1){
		printf("fork error\n");
		exit(-1);
	}
	if(pid==0){
		produttore(key);
	}
	else{
		wait(NULL);
		shm_ds=shmget(key,SIZE,0);
		message=shmat(shm_ds,NULL,0);
		printf("[Main thread] I read: %s\n",(char*)message);
		exit(0);
	}

}
