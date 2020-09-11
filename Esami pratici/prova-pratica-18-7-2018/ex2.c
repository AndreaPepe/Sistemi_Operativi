#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>


int sem_ds_write;
int sem_ds_read;

char buffer[1024];
__thread char *filename;
char **path_names;

void handler(int signo){
	
	char buff[128];
	sprintf(buff,"cat %s",filename);
	system(buff);

	return;
}


void* thr_funct(void* arg){

	int index = (int) arg;
	int fd;
	int ret;
	struct sembuf oper;

	filename=path_names[index];	//TLS
	
	fd=open(filename,O_CREAT|O_TRUNC|O_RDWR,0666);
	if(fd==-1){
		printf("error opening the file %s\n",filename);
		exit(-1);
	}
	while(1){
		oper.sem_num = index-1;
		oper.sem_op = -1;
		oper.sem_flg = 0;

retry1:	
		ret=semop(sem_ds_read,&oper,1);
		if(ret==-1){
			if(errno==EINTR)
				goto retry1;
			else{
				printf("semop() error\n");
				exit(-1);
			}
		}

retry2:	
		ret=write(fd,buffer,strlen(buffer)+1);
		if(ret==-1){
			if (errno==EINTR)
				goto retry2;
			else{
				printf("write error\n");
				exit(-1);
			}
		}

		oper.sem_num=0;
		oper.sem_op=1;
		oper.sem_flg=0;

retry3:
		ret=semop(sem_ds_write,&oper,1);
		if(ret==-1){
			if(errno==EINTR)
				goto retry3;
			else{
				printf("semop() error\n");
				exit(-1);
			}
		}
	}
	pthread_exit((void*)0);	
}




int main(int argc, char **argv){
	
	int i, ret;
	int num_thr;
	pthread_t tid;

	if(argc<2){
		printf("Usage: prog pathname_1 <pathname_2> ... <pathname_N>\n");
		exit(-1);
	}

	path_names=argv;
	num_thr=argc-1;
	filename=argv[1];

	sem_ds_read=semget(IPC_PRIVATE,num_thr,O_CREAT|0666);
	if(sem_ds_read==-1){
		printf("Error creating read semaphore\n");
		exit(-1);
	}

	sem_ds_write=semget(IPC_PRIVATE,1,O_CREAT|0666);
	if(sem_ds_write==-1){
                printf("Error creating write semaphore\n");
                exit(-1);
        }

	/*initialization*/

	ret=semctl(sem_ds_write,0,SETVAL,1);
	if(ret==-1){
		printf("Error initializating write semaphore\n");
		exit(-1);
	}
	
	for(i=0;i<num_thr;i++){
		ret=semctl(sem_ds_read,i,SETVAL,0);
		if(ret==-1){
			printf("Error initializating read semafophore\n");
			exit(-1);
		}
	}

	signal(SIGINT,handler); 	//signal management

	for(i=0;i<num_thr;i++){
		ret=pthread_create(&tid,NULL,thr_funct,(void*)(i+1));
		if(ret==-1){
			printf("Error creating thread %d\n", i+1);
			exit(-1);
		}
	}	

	struct sembuf oper;

	while(1){
		oper.sem_num = 0;
		oper.sem_op = -num_thr;
		oper.sem_flg =0;

retry:		ret=semop(sem_ds_write,&oper,1);
		if(ret==-1){
			if(errno==EINTR)
				goto retry;
			else{
				printf("semop() error\n");
				exit(-1);
			}
		}

retry2:		ret=scanf("%s",buffer); // global defined
		if(ret==EOF){
			if(errno==EINTR)
				goto retry2;
			else{
				printf("scanf error\n");
				exit(-1);
			}
		}

		oper.sem_op=1;
		oper.sem_flg=0;
		for(i=0;i<num_thr;i++){
			oper.sem_num = i;

retry3:			ret=semop(sem_ds_read,&oper,1);
			if(ret==-1){
				if(errno==EINTR)
					goto retry3;
				else{
					printf("semop() error\n");
					exit(-1);
				}
			}
		}
	}/*end while*/

	return 0; /*end main*/

}
