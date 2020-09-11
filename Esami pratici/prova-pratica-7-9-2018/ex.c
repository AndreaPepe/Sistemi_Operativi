#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <sys/sem.h>
#include <pthread.h>
#include <fcntl.h>

#define PG_SIZE 4096

FILE* file;
char* filename;
long num_threads;
int fd;
int sem_1;
int sem_2;
char **sh_memory;
pid_t pid;

void parent_handler(int signo){
	kill(pid,signo);
}

void child_handler(int signo){
	char buffer[128];
	sprintf(buffer,"cat %s\n",filename);

	system(buffer);
}


void* parent_thread(void* arg){
	int ret;
	long me=(long)arg;
	struct sembuf oper;

	oper.sem_num = me;
	oper.sem_flg = 0;
	while(1){
		oper.sem_op = -1;
retry1:
		ret=semop(sem_1,&oper,1);
		if(ret==-1){
				if(errno==EINTR)
				goto retry1;
			else{
				printf("semop error\n");
				exit(-1);
			}
		}

retry2:
		ret=scanf("%s",sh_memory[me]);
		if(ret==-1){
			if(errno==EINTR)
				goto retry2;
			else{
				printf("scanf error\n");
				exit(-1);
			}
		}

		oper.sem_op=1;
retry3:
		ret=semop(sem_2,&oper,1);
                if(ret==-1){
                        if(errno==EINTR)
                                goto retry1;
                        else{
                                printf("semop error\n");
                                exit(-1);
                        }
                }
	}
	return NULL;
}




void* child_thread(void* arg){

	int ret;
	long me=(long) arg;
	struct sembuf oper;
        
	oper.sem_num = me;
        oper.sem_flg = 0;
        
	while(1){
                oper.sem_op = -1;
retry1:
                ret=semop(sem_2,&oper,1);
                if(ret==-1){
                                if(errno==EINTR)
                                goto retry1;
                        else{
                                printf("semop error\n");
                                exit(-1);
                        }
                }
		//printf("%s\n",sh_memory[me]);
		fprintf(file,"%s\n",sh_memory[me]);
                fflush(file);

               oper.sem_op=1;
retry3:
               ret=semop(sem_1,&oper,1);
               if(ret==-1){
                        if(errno==EINTR)
                                goto retry1;
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
	//pid_t pid;
	pthread_t tid;

	if(argc!=3){
		printf("Usage: %s filename num_threads\n",argv[0]);
		exit(-1);
	}
	
	filename=argv[1];
	fd=open(filename, O_CREAT|O_TRUNC|O_RDWR,0666);
	if(fd==-1){
		printf("open error\n");
		exit(-1);
	}	
	file=fdopen(fd,"w+");

	num_threads=strtol(argv[2],NULL,10);
	if(num_threads<1){
		printf("ERROR: num_threads<1 \n");
		exit(-1);
	}
	
	sh_memory=malloc(num_threads*sizeof(char*));
	if(sh_memory==NULL){
		printf("malloc error\n");
		exit(-1);
	}
	for(i=0;i<num_threads;i++){
		sh_memory[i]=(char*)mmap(NULL,PG_SIZE,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
		if(sh_memory[i]==NULL){
			printf("mmap error\n");
			exit(-1);
		}	
	}

	sem_1=semget(IPC_PRIVATE,num_threads,O_CREAT|0666);
	if(sem_1==-1){
		printf("semget error\n");
		exit(-1);
	}
	
	sem_2=semget(IPC_PRIVATE,num_threads,O_CREAT|0666);
        if(sem_2==-1){
                printf("semget error\n");
                exit(-1);
        }

	for(i=0;i<num_threads;i++){
		ret=semctl(sem_1,i,SETVAL,1);
		if(ret==-1){
			printf("semctl error\n");
			exit(-1);
		}

		ret=semctl(sem_2,i,SETVAL,0);
                if(ret==-1){
                        printf("semctl error\n");
                        exit(-1);
                }
	}

	pid=fork();
	if(pid==-1){
		printf("fork error\n");
		exit(-1);
	}

	if(pid==0){
		signal(SIGINT, child_handler);
		
		for(i=0;i<num_threads;i++){
			ret=pthread_create(&tid,NULL,child_thread,(void*)i);
			if(ret==-1){
				printf("pthread_create error\n");
				exit(-1);
			}
		}
	
	}
	else{
		signal(SIGINT, parent_handler);

                for(i=0;i<num_threads;i++){
                        ret=pthread_create(&tid,NULL,parent_thread,(void*)i);
                        if(ret==-1){
                                printf("pthread_create error\n");
                                exit(-1);
                        }
                }
	}
	
	while(1) pause();
}
