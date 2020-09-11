#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define SIZE 4096
#define TYPE 5

typedef struct _message{
	long type;
	char mex[SIZE];
} message;


int ds_q;

void producer(int ds_q){
	message msg;
	int ret;

	printf("Insert strings ('quit' to close):\n");
	fflush(stdout);
	do{
		scanf("%s", msg.mex);
		msg.type = TYPE;
		ret=msgsnd(ds_q,&msg,SIZE,IPC_NOWAIT);	// non bloccante
		if(ret==-1){
			printf("msgsnd error\n");
			exit(-1);
		}
	}while((strcmp(msg.mex, "quit"))!=0);

	exit(0);
}


void consumer(int ds_q){
	message msg_rcv;
	int ret;

	do{
		ret=msgrcv(ds_q,&msg_rcv,SIZE,TYPE,0);
		if(ret==-1){
			printf("msgrcv error\n");
			exit(-1);
		}
		printf("%s\n",msg_rcv.mex);
		fflush(stdout);
	}while((strcmp(msg_rcv.mex, "quit"))!=0);

	exit(0);

}

int main(int argc, char **argv){

	key_t key = 30;
	pid_t pid;
	int ret;
	int status;

	ds_q=msgget(key,IPC_CREAT|0666);
	if(ds_q==-1){
		printf("msgget error\n");
		exit(-1);
	}

	pid=fork();
	if(pid==-1){
		printf("fork error\n");
		exit(-1);
	}
	if(pid==0)
		consumer(ds_q);

	else{
		pid=fork();
		if(pid==-1){
			printf("fork error\n");
			exit(-1);
		}
		if(pid==0)
			producer(ds_q);
		else{
			wait(&status);
			wait(&status);
		}
	}

	ret=msgctl(ds_q,IPC_RMID,0);
	if(ret==-1){
		printf("Unable to remove msg queue\n");
		exit(-1);
	}
	return 0;



}
