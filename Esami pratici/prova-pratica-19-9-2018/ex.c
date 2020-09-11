#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/types.h>

#define SIZE 5

char **filenames;
long num_threads;
int sem_r;
int sem_w;

char buff[SIZE];

/*void handler(int signo){
	
	char buffer[128];
	int ret;
	int i;

	printf("\nHandler in action:\n\n");
	fflush(stdout);

	for(i=0;i<num_threads;i++){
		memset(buffer, 0, 128);
		sprintf(buffer,"cat %s", filenames[i]);
		system(buffer);
	}

	return;
}*/

void handler(int signo){
	int ret,i,j;
	int fd[num_threads];
	char buffer[SIZE];

	printf("\nHandler alive: \n");
	fflush(stdout);
	
	for(i=0;i<num_threads;i++){
		fd[i]=open(filenames[i],O_RDONLY);
		lseek(fd[i],0,SEEK_SET);
	}
	
	i=0;
	do{
		ret=read(fd[i],buffer,SIZE);
		write(1,buffer,ret);			
		i=(i+1)%num_threads;
	}while(ret);
	
	return;
}


void* thread_f(void *arg){
	
	long me;
	int ret;
	int fd;
	struct sembuf oper;

	me=(long)arg;
	fd=open(filenames[me],O_RDWR);
	if(fd==-1){
		printf("error open by thread\n");
		exit(-1);
	}
	
	while(1){
		oper.sem_num=me;
		oper.sem_op=-1;
		oper.sem_flg=0;

retry1:		ret=semop(sem_r, &oper, 1);
		if(ret==-1){
			if(errno == EINTR) goto retry1;
			else{
				printf("semop error\n");
				exit(-1);
			}
		}

retry2:		ret=write(fd,buff,SIZE);
		if(ret==-1){
                        if(errno == EINTR) goto retry2;
                        else{
                                printf("write error\n");
                                exit(-1);
                        }
                }


		oper.sem_num = (me+1)%num_threads;
		oper.sem_op = 1;
retry3:		
		ret=semop(sem_w,&oper,1);
		if(ret==-1){
                        if(errno == EINTR) goto retry3;
                        else{
                                printf("semop error\n");
                                exit(-1);
                        }
                }
	}
	close(fd);
	return NULL;

}

int main(int argc, char **argv){
	
	int i,ret;	
	struct sembuf oper;
	pthread_t tid;
	int file;
	if(argc<2){
		printf("Usage: %s filename1 <filename2> ... <filenameN>\n",argv[0]);
		exit(-1);
	}
	
	num_threads=(long)argc-1;
	filenames = &argv[1];
	

	for(i=0;i<num_threads;i++){
		file=open(filenames[i],O_CREAT|O_TRUNC|O_RDWR, 0666);
		if(file == -1){
			printf("Error opening file %d\n",i+1);
			exit(-1);
		}
	}
	
	sem_r=semget(IPC_PRIVATE,num_threads,IPC_CREAT|0666);
	if(sem_r == -1){
		printf("semget error\n");
		exit(-1);
	}

	sem_w=semget(IPC_PRIVATE,num_threads,IPC_CREAT|0666);
        if(sem_w == -1){
                printf("semget error\n");
                exit(-1);
        }
		

	ret=semctl(sem_w,0,SETVAL,1);
	if(ret==-1){
		printf("semctl error\n");
		exit(-1);
	}

	for(i=1;i<num_threads;i++){
		ret=semctl(sem_w,i,SETVAL,0);
	        if(ret==-1){
        	        printf("semctl error\n");
                	exit(-1);
        	}
	}

	for(i=0;i<num_threads;i++){

		ret=semctl(sem_r,i,SETVAL,0);
        	if(ret==-1){
                	printf("semctl error\n");
                	exit(-1);
        	}
	}
	
	signal(SIGINT,handler);

	for(i=0;i<num_threads;i++){
		ret=pthread_create(&tid, NULL, thread_f, (void*)((long)i));
		if(ret==-1){
			printf("pthread_create error\n");
			exit(-1);
		}	
	}

	i=0;
	int j;
	while(1){
		oper.sem_num = i;
		oper.sem_op = -1;
		oper.sem_flg = 0;

retry1:		ret=semop(sem_w,&oper,1);
		if(ret==-1){
			if(errno==EINTR) goto retry1;
			else{
				printf("semop error\n");
				exit(-1);
			}
		}

		for(j=0;j<SIZE;j++)
			buff[j]=getchar();
	

		oper.sem_op = 1;
retry2:	
		ret=semop(sem_r,&oper,1);
                if(ret==-1){
                        if(errno==EINTR) goto retry2;
                        else{
                                printf("semop error\n");
                                exit(-1);
                        }
                }
		

		i=(i+1)%num_threads;
	}
	
	semctl(sem_r,IPC_RMID,0);
	semctl(sem_w,IPC_RMID,0);
	return 0;

}
