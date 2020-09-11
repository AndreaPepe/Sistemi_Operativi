#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

#define N 4

void main(int argc, char *argv[])
{
	pid_t pid;
	int i, status, forked = 0;

	char path[4][256];

	for (i=0; i<N; i++)
	{
		printf("Enter path %d: ", (i+1));
		scanf("%s", path[i]);
	}

	for (i=0; i<N; i++)
	{
		pid = fork();

		if (pid == -1) {
			printf("Unable to fork new process\n");
		} else if (pid == 0) {
			execlp("ls", "ls", path[i], NULL);
			exit(-1);
		} else {
			forked += 1;
		}
	}

	for (i=0; i<forked; i++)
	{
		wait(&status);
	}
}