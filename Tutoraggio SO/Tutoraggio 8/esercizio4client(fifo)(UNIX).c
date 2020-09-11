#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

void * client(void *arg)
{
	int fd;
	ssize_t bytes;
	int num = 0;

	char *fifonamec;
	char message[32];

	if ((fifonamec = (char *) arg) == NULL)
	{
		free(arg);
		pthread_exit((void *) -1);
	}

	while (num < 10)
	{
		/*
		 * Apriamo la FIFO per leggere un messaggio
		 * dal server-thread.
		 */
		if ((fd = open(fifonamec, O_RDONLY)) < 0)
		{
			free(arg);
			pthread_exit((void *) -1);
		}

		while ((bytes = read(fd, message, 32)) == 0)
			sleep(1);

		close(fd);

		num = atoi(message);

		printf("Client Thread: %d\n", num);

		if (snprintf(message, 32, "%d", ++num) < 0)
		{
			free(fifonamec);
			pthread_exit((void *) -1);
		}

		/*
		 * Apriamo la FIFO per scrivere un messaggio
		 * di risposta al server-thread.
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
	}

	free(arg);
	pthread_exit((void *) 0);
}

int main(int argc, char *argv[])
{
	int i;
	int nt;
	int fd;
	int nthreads;

	char *arg;
	char fifonamec[32];

	const char *fifonames = "server_fifo";

	if (argc < 2)
		return -1;

	/*
	 * Il numero di client-thread da utilizzare è l'argomento
	 * del programma client e deve essere almeno pari a 1.
	 */
	if ((nt = atoi(argv[1])) < 1)
		return -1;

	pthread_t threads[nt];

	/*
	 * Apriamo in scrittura la FIFO del processo server il cui nome, in
	 * maniera analoga ad un indirizzo IP di un server sulla rete, è
	 * conosciuto system-wide.
	 */
	if ((fd = open(fifonames, O_WRONLY)) < 0)
		return -1;

	for (i=0, nthreads=0; i<nt; i++)
	{
		/*
		 * Per ogni client-thread produciamo un nome univuco per la
		 * FIFO con cui potrà comunicare con un server-thread.
		 */
		if (snprintf(fifonamec, 32, "client_fifo_%d", nthreads) < 0)
			continue;

		/*
		 * Creiamo la FIFO se non è già esistente.
		 */
		if (mkfifo(fifonamec, S_IRUSR|S_IWUSR) && errno != EEXIST)
			continue;

		/*
		 * Comunichiamo al server il nome della nuova FIFO tramite
		 * la quale un client-thread vorrà comunicare.
		 */
		if (write(fd, fifonamec, 32) < 0)
			continue;

		if ((arg = strdup(fifonamec)) == NULL)
			continue;

		/*
		 * Generiamo il client-thread che comunicherà per mezzo dell'appena
		 * creata FIFO.
		 */
		if (pthread_create(&threads[nthreads], NULL, client, (void *) arg) == 0)
			nthreads++;
	}

	close(fd);

	for (i=0; i<nthreads; i++)
		pthread_join(threads[i], NULL);

	return 0;
}