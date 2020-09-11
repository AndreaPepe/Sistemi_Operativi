#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char *filename = "read-write.txt";

void main(int argc, char *argv[])
{
	int fd;
	size_t len;
	pid_t pid;

	fd = open(filename, O_CREAT|O_RDWR|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
	if (fd == -1) {
		printf("Unable to create and open file %s\n", filename);
		exit(-1);
	}

	pid = fork();
	if (pid == -1) {
		printf("Unable to fork new process\n");
		exit(-1);
	}

	if (pid == 0)
	{
		char *text;

		printf("Give me a message to pass to the parent process: ");
		if (scanf("%m[^\n]", &text) == -1) {
			exit(-1);
		}

		len = strlen(text);

		if (write(fd, text, len) == -1) {
			printf("Unable to write on file\n");
			exit(-1);
		}

		free(text);

		close(fd);

		exit(len);
	}
	else
	{
		int aux;
		int status;
		
		if (wait(&status) == -1) {
			printf("Wait has failed\n");
			exit(-1);
		} else if ((aux = WEXITSTATUS(status)) == -1) {
			printf("Child process terminated with error\n");
			exit(-1);
		}

		len = (size_t) aux;

		char text[len + 1];

		/*
		 * Attenzione! Il processo padre ha aperto il file prima di
		 * di invocare la fork.
		 *
		 * Il processo figlio ha una vista dei descrittori dei files
		 * che è un clone di quella del processo padre.
		 *
		 * Entrambe fanno riferimento alla stessa sessione.
		 *
		 * La ascrittura su file da parte del processo figlio fa
		 * avanzare il puntotatore all'interno del file.
		 *
		 * Per accedere al messaggio scritto dal figlio, il processo
		 * padre devre riposizionare il puntatore all'inizio del file.
		 */
		lseek(fd, 0, SEEK_SET);

		if (read(fd, text, len) == -1) {
			printf("Unable to read from file\n");
			exit(-1);
		}

		text[len] = '\0';

		close(fd);

		printf("Child process gave me the message: %s\n", text);

		exit(0);
	}
}
