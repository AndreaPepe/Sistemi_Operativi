#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

void * server(void *arg)
{
	int fd;
	ssize_t bytes;
	int num = 0;

	char *fifonamec;
	char message[32];

	if ((fifonamec = (char *) arg) == NULL)
		pthread_exit((void *) -1);

	while (num < 10)
	{
		if (snprintf(message, 32, "%d", ++num) < 0)
		{
			free(fifonamec);
			pthread_exit((void *) -1);
		}

		/*
		 * Apriamo la FIFO per scrivere un messaggio al
		 * client-thread in attesa di una risposta da
		 * parte del server.
		 */
		if ((fd = open(fifonamec, O_WRONLY)) < 0)
		{
			free(fifonamec);
			pthread_exit((void *) -1);
		}

		if (write(fd, message, 32) < 0)
		{
			close(fd);
			free(fifonamec);
			pthread_exit((void *) -1);
		}

		close(fd);

		/*
		 * Apriamo la FIFO per leggere un messaggio di
		 * ritorno dal client-thread.
		 */
		if ((fd = open(fifonamec, O_RDONLY)) < 0)
		{
			free(fifonamec);
			pthread_exit((void *) -1);
		}

		while ((bytes = read(fd, message, 32)) == 0)
			sleep(1);

		close(fd);

		num = atoi(message);

		printf("Server Thread: %d\n", num);
	}

	free(fifonamec);
	pthread_exit((void *) 0);
}

int main(int argc, char *argv[])
{
	int fd;
	ssize_t bytes;
	pthread_t thread;

	char * arg;
	char fifonamec[32];

	const char *fifonames = "server_fifo";

	/*
	 * In caso non esista già il file speciale, creaiamo una
	 * FIFO per il processo server, il cui nome è univuco e
	 * conosciuto system-wide.
	 */
	if (mkfifo(fifonames, S_IRUSR|S_IWUSR) && errno != EEXIST)
		return -1;

	/*
	 * Mettiamo quindi il processo server in ascolto per l'apertura
	 * della FIFO da parte di qualche scrittore.
	 *
	 * Se non diversamente specificato (O_NONBLOCK) l'apertura in lettura
	 * di una FIFO è bloccante in attesa dell'apertura di almeno uno
	 * scrittore e viceversa.
	 */
	if ((fd = open(fifonames, O_RDONLY)) < 0)
		return -1;

	while (1)
	{
		/*
		 * Attendiamo per la disponibilità del dato sulla FIFO. L'operazione
		 * di lettura è non-bloccante, motivo per cui ci riserviamo del tempo
		 * per attendere la disponibilità del dato nel caso in cui non fosse
		 * già disponibile.
		 */
		while ((bytes = read(fd, fifonamec, 32)) == 0)
			sleep(1);

		/*
		 * Il processo server riceve solo messaggi contenenti stringhe che
		 * rappresentano il nome delle FIFO private di ogni client-thread.
		 */
		if ((arg = strdup(fifonamec)) == NULL)
		{
			close(fd);
			return -1;
		}

		/*
		 * Creiamo un server-thread apposito per gestire separatamente la
		 * comunicazione con ogni client-thread.
		 *
		 * Il processo server è non-aware del numero di client-thread e
		 * relative FIFO che vorranno comunicare. Esso si limita a reperire
		 * il nome di una nuova FIFO all'interno del while-loop e delegare
		 * un server-thread per la sua gestione.
		 */
		if (pthread_create(&thread, NULL, server, (void *) arg))
		{
			free(arg);
			close(fd);
			return -1;
		}
	}

	return 0;
}