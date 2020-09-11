#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>

#define SIZE 4096
#define N 2

int arg;
int fd;
int semaphore;
void **mem;
char *file;


void error (char* x){
	printf("x");
	exit(-1);
}


void handler(int signo, siginfo_t* a, void* b){
	int i;

	for(i=0;i<N;i++){
		file=mem[i];

		while(strcmp(file,"\0")!=0){
			printf("%s\n",file);
			file+=strlen(file) + 1;
		}
	}
}


void child_process(){
	FILE* f;
	f = fdopen(fd, "r");
	struct sembuf oper;

	if (f==NULL)
		error("fdopen error\n");

	while(fscanf(f,"%s",file)!=EOF){
		file+=strlen(file)+1;
	}
	
	oper.sem_num = 0;
	oper.sem_op = 1;
	oper.sem_flg = 0;

	semop(semaphore,&oper,1);

	while(1) pause();
}

int main(int argc, char** argv){

	int i;
	pid_t pid;
	sigset_t set;
	struct sigaction act;

	arg=argc; //saving in a global variable to print files

	if(argc<N+1){
		error("Usage: prog <file1> <file2>\n");
	}
	


	for(i=1;i<N;i++){
		if(fd=open(argv[i],O_RDONLY)==-1)
			error("file not existing\n");
	
	}  
	
	semaphore=semget(IPC_PRIVATE,1,0666);
	if(semaphore==-1)
		error("Error creating semaphore\n");

	semctl(semaphore,0,SETVAL,0);

	mem=malloc(N * sizeof(void*));
	if(mem==NULL)
		error("Malloc error\n");

	sigfillset(&set);
	act.sa_sigaction = handler;
	act.sa_mask=set;
	act.sa_flags=0;

	sigaction(SIGINT,&act,NULL);

	for(i=0;i<N;i++){
		mem[i]=mmap(NULL,SIZE,0666,MAP_ANONYMOUS|MAP_SHARED,0,0);
		if(mem[i]==NULL)
			error("mmap error\n");

		file=mem[i];
		fd=open(argv[i+1],O_RDONLY);
		pid=fork();
		if(pid==-1)
			error("fork error\n");
		if(pid==0){
			signal(SIGINT,SIG_IGN);
			child_process();
		}
	}

	struct sembuf oper;
	int ret;

	oper.sem_num = 0;
	oper.sem_op = -N;
	oper.sem_flg = 0;
step1:
	ret=semop(semaphore,&oper,1);
	if(ret==-1 && errno==EINTR)	// syscall aborted
		goto step1;
	if(ret==-1 && errno!=EINTR)
		error("semop error\n");


	while(1) pause();
	exit(0);
}

