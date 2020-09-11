#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

void main(int argc, char *argv[])
{
	pid_t pid;
	int status;

#ifdef ONSTACK
	char text[1024];
#else
	char *text1;
#endif

	printf("Enter any string: ");

#ifdef ONSTACK
	scanf("%[^\n]", text);
#else
	scanf("%m[^\n]", &text1);
#endif

	pid = fork();

	if (pid == -1) {
		printf("Unable to fork new process\n");
		exit(-1);
	}

	if (pid == 0) {
#ifdef ONSTACK
		printf("Process with PID=%d inhered the string into the STACK: %s\n", getpid(), text);
#else
		printf("Process with PID=%d inhered the string into the HEAP: %s\n", getpid(), text1);
#endif
		exit(0);
	} else {
		wait(&status);
		printf("Process with PID=%d has finished\n", getpid());
	}
}