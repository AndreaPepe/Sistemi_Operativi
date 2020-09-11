#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define N 8
#define W 100

static const char *filename = "file-lock.txt";

void main(int argc, char *argv[])
{
	int i;
	int fd;
	int len;
	int status;
	int p_created;
	pid_t pid;
	char text[10];
	struct flock fl;

	if ((fd = open(filename, O_CREAT|O_RDWR|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO)) == -1) {
		printf("Unable to create and open file %s\n", filename);
		exit(-1);
	}

	p_created = N;

	for (i=0; i<N; i++) {

		if ((pid = fork()) == -1) {

			printf("Unable to fork process\n");
			p_created--;

		} else if (pid == 0) {

			pid = getpid();
			len = sprintf(text, "%d ", (int)pid);

			/*
			 * Advisory Record Locking
			 * -----------------------
			 * Linux implementa un sistema UNIX recod lock per files
			 * come previsto dalle specifiche POSIX.
			 *
			 * Permette di definire ed utilizzare dei locks per un
			 * sotto-insieme di records del file o tutto il file.
			 *
			 * l_type = FWRLCK : richiede un write-lock sul file.
			 * l_type = F_UNLCK : richiede il rilascio del lock.
			 *
			 * l_whnce = SEEK_SET, l_start = 0, l_len = 0 : il lock
			 * è definito per tutti i records che vanno dall'inizio
			 * del file alla sua fine, indipendentemente da quanto
			 * questo possa crescere.
			 *
			 * F_SETLKW : richiede di settare il lock e, in caso fosse
			 * già acquisito, attende il suo rilascio.
			 */
			fl.l_type = F_WRLCK;
			fl.l_whence = SEEK_SET;
			fl.l_start = 0;
			fl.l_len = 0;
			fl.l_pid = 0;

			if (fcntl(fd, F_SETLKW, &fl) == -1) {  
				printf("Unable to lock file %s\n", filename);
				exit(-1);
			}

			for (i=0; i<W; i++) {
				if (write(fd, text, (size_t)len) == -1) {
					printf("Unable to write on file\n");
				}
			}
			if (write(fd, "\n\n", 2) == -1) {
				printf("Unable to write on file\n");
			}

			fl.l_type = F_UNLCK;

			if (fcntl(fd, F_SETLKW, &fl) == -1) {  
				printf("Unable to unlock file %s\n", filename);
				exit(-1);
			}

			close(fd);
			exit(0);

		}

	}

	for (i=0; i<p_created; i++) {
		if (wait(&status) == -1) {
			printf("Wait has failed\n");
		}
	}

	close(fd);
	exit(0);
}