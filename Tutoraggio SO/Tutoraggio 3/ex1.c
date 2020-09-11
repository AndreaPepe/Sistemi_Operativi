#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main (int argc, char **argv){

	printf("[Main process-pid %d] Ready to acquire a line:\n", getpid());
	fflush(stdout);
	char *ptr;
	scanf("%m[^\n]", &ptr);
	
	pid_t pid;

	if((pid=fork())==-1){
		printf("Fork() error!\n");
		exit(-1);
	}

	if(!pid){
		printf("[Child process-pid: %d] Acquired line is : %s\n", getpid(), ptr);
		fflush(stdout);

		printf("Child process exiting with pid-%d\n",getpid());

	}
	else{
		wait(NULL);
		printf("Main process exiting with pid-%d\n",getpid());
		return 0;
	}
}
