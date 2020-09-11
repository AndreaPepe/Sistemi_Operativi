#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>


char **strings;
char *filename;
long num_thr;
int fd;
int sem_thr, sem_main, sem_w;
char buffer[4096];
FILE* file;

void handler(int signo){
	char buff[128];
	printf("\nHandler in action: \n");
	fflush(stdout);
	
	sprintf(buff,"cat %s", filename);
	system(buff);
	return;
}


void* thread_function(void *arg){

	int ret,len,i;
	long me;
	struct sembuf oper;

	me=(long) arg;

	while(1){
		oper.sem_num = me;
		oper.sem_op = -1;
		oper.sem_flg = 0;
retry1:
		ret=semop(sem_thr,&oper,1);
		if(ret==-1){
			if(errno==EINTR) goto retry1;
			else{
				printf("semop error\n");
				exit(-1);
			}
		}
		len=strlen(strings[me]);
		if(strcmp(buffer,strings[me])==0){
			for(i=0;i<len;i++)
				buffer[i]='*';
		}
		oper.sem_num = 0;
		oper.sem_op = 1;
retry2:

		ret=semop(sem_w,&oper,1);
                if(ret==-1){
                        if(errno==EINTR) goto retry2;
                        else{
                                printf("semop error\n");
                                exit(-1);
                        }
                }
		
		oper.sem_num = 0;
		oper.sem_op = 1;
retry3:
		ret=semop(sem_main,&oper,1);
                if(ret==-1){
                        if(errno==EINTR) goto retry3;
                        else{
                                printf("semop error\n");
                                exit(-1);
                        }
                }

	}
	return NULL;
}


int main(int argc, char **argv){
	
	int i, ret;
	pthread_t tid;
	struct sembuf oper;

	if(argc<3){
		printf("Usage: %s filename string1 <string2> ... <stringN>\n",argv[0]);
		exit(-1);
	}
	
	filename=argv[1];
	num_thr=(long)argc - 2;
	strings=(char**)malloc(num_thr*sizeof(char*));
	if(strings==NULL){
		printf("malloc failure\n");
		exit(-1);
	}
	
	for(i=0;i<num_thr;i++){
		strings[i]=argv[i+2];
	}

	fd=open(filename, O_CREAT|O_TRUNC|O_RDWR,0666);
	if(fd==-1){
		printf("open error\n");
		exit(-1);
	}
	
	file=fopen(filename, "w+");

	sem_main=semget(IPC_PRIVATE,1,O_CREAT|0666);
	if(sem_main==-1){
		printf("semget error\n");
		exit(-1);
	}
	
	sem_thr=semget(IPC_PRIVATE,num_thr,O_CREAT|0666);
        if(sem_thr==-1){
                printf("semget error\n");
                exit(-1);
        }
	
	sem_w=semget(IPC_PRIVATE,1,O_CREAT|0666);
        if(sem_w==-1){
                printf("semget error\n");
                exit(-1);
        }

	ret=semctl(sem_main,0,SETVAL,num_thr);
	if(ret==-1){
		printf("semctl error\n");
		exit(-1);
	}

	for(i=0;i<num_thr;i++){
		ret=semctl(sem_thr,i,SETVAL,0);
		if(ret==-1){
			printf("semctl error\n");
			exit(-1);
		}
	}

	ret=semctl(sem_w,0,SETVAL,0);
        if(ret==-1){
                printf("semctl error\n");
                exit(-1);
        }


	for(i=0;i<num_thr;i++){
		ret=pthread_create(&tid,NULL,thread_function,(void*)((long)i));
		if(ret==-1){
			printf("pthread_create error\n");
			exit(-1);
		}
	}

	signal(SIGINT, handler);

	while(1){
		i=0;
		oper.sem_num = 0;
		oper.sem_op = -num_thr;
		oper.sem_flg = 0;
retry1:
		ret=semop(sem_main,&oper,1);
		if(ret==-1){
			if(errno==EINTR) goto retry1;
			else{
				printf("semop error\n");
				exit(-1);
			}
		}

retry2:		
		ret=scanf("%s",buffer);
		if(ret==-1){
			if(errno==EINTR) goto retry2;
			else{
				printf("semop error\n");
				exit(-1);
			}
		}

		oper.sem_op = 1;
		for(i=0;i<num_thr;i++){
retry3:			oper.sem_num = i;
			ret=semop(sem_thr,&oper,1);
            	    	if(ret==-1){
                        	if(errno==EINTR) goto retry3;
                        	else{
                                	printf("semop error\n");
                                	exit(-1);
                        	}
               		 }	
		}

		oper.sem_num = 0; 
		oper.sem_op = -num_thr;
retry4:	
		ret=semop(sem_w,&oper,1);
		if(ret==-1){
			if(errno==EINTR) goto retry4;
			else{
				printf("semop error\n");
				exit(-1);
			}
		}
		
		fprintf(file,"%s\n",buffer);
		fflush(file);
	}

	return 0;
}
