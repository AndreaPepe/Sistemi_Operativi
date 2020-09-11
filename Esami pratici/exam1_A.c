#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/types.h>


typedef struct _data{
	int val;
	struct _data* next;
}data;


long num_threads;
data* lists;
pthread_mutex_t lock;
pthread_mutex_t next;
int val;		// sezione critica

void* thread_funct (void* id){
	
	data* record;
	int ret;
	
	while(1){
		record=malloc(sizeof(data));
		if(record==NULL){
			printf("malloc error\n");
			exit(-1);
		}

		ret=pthread_mutex_lock(&lock);
		
		if(ret!=0){
			printf("error locking\n");
			exit(-1);
		}
		
		printf("[Thread %d] found value: %d\n",(int)id, val);

		record->val=val;
		ret=pthread_mutex_unlock(&next);
		if(ret!=0){
                        printf("error locking\n");
                        exit(-1);
                }
		record->next = lists[(int)id].next;
		lists[(int)id].next=record; 
	}

	return((void*)0);

}


void stampa(int signo, siginfo_t *a, void* b){
	
	data record;
	int i;

	for(i=0;i<num_threads;i++){
		printf("printing list%d\n",i);
		record=lists[i];
		while(record.next){
			printf("%d ",record.val);
			record= *(record.next);
		}
		printf("\n");
	}

}



int main(int argc, char** argv){

	int i; int ret;
	pthread_t tid;
	
	if (argc!=2){
		printf("Usage: ./a.out <num threads> \n");
		exit(-1);
	}

	num_threads=strtol(argv[1], NULL, 10);
	lists=malloc(num_threads*sizeof(data));
	if(lists==NULL){
		printf("malloc error\n");
		exit(-1);
	}

	for(i=0;i<num_threads;i++){
		lists[i].val=-1;
		lists[i].next=NULL;
	}

	ret=pthread_mutex_init(&lock,NULL);
	if(ret!=0){
		printf("lock init error\n");
		exit(-1);
	}

	ret=pthread_mutex_init(&next,NULL);
        if(ret!=0){
                printf("next init error\n");
                exit(-1);
        }

	ret=pthread_mutex_lock(&lock);
        if(ret!=0){
                printf("locking error\n");
                exit(-1);
        }
	
	/* SIGINT management */
	sigset_t set;
	struct sigaction act;

	sigfillset(&set);
	act.sa_sigaction = stampa;
	act.sa_mask = set;
	act.sa_flags = 0;

	sigaction(SIGINT,&act,NULL);

	/* spawning threads */
	printf("[Main] Spawning threads\n\n");
	for(i=0;i<num_threads;i++){
		ret = pthread_create(&tid,NULL,thread_funct,(void*)i);		      if(ret!=0){
			printf("pthread_create %d error\n",i);
			exit(-1);
		}	
	}


	while(1){
	
step1:
		ret=pthread_mutex_lock(&next);
		if(ret!=0 && errno==EINTR) goto step1;
step2:
		ret=scanf("%d",&val);
		if(ret==EOF) goto step2;
		if(ret==0){
			printf("not integer value inserted\n");
			exit(-1);
		}	
step3:
		ret=pthread_mutex_unlock(&lock);
		if(ret!=0 && errno==EINTR) goto step3;
	
	}
	return 0;

}
