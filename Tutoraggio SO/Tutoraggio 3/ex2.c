#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define N 2

int main(int argc, char **argv){
	
	pid_t pid; int status, i;
	char* path[N];
	
	for(i=0;i<N;i++){
		printf("Insert a path: ");
		scanf("%s\n",path[i]);
	} 

	for(i=0;i<N;i++){
		pid=fork();
		if(pid==-1){
			printf("Fork error occurred\n");
			exit(-1);
		}

		else if(pid==0){
			printf("Child %d:\n",getpid());
			fflush(stdout);
			execlp("ls","ls", path[i], NULL);		
		}
	}

	for(i=0;i<N;i++)
		wait(&status);

	printf("Main process exiting\n");
	return 0;
}
