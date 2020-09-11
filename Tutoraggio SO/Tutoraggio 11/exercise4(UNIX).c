#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define N_STEP	4

#define N_READER	8

#define SHMEM_MSG_KEY	6832
#define SHMEM_MSG_SIZE	4096

#define SEMAPHORE_KEY	6842


union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};


void writer_function(void);
void reader_function(void) __attribute__((noreturn));


int msgid;
int semid;

char * message;


void reader_function(void)
{
	struct sembuf buf[1];

	/*
	 * Inizialmente i Lettori devono attendere alla barriera
	 * imposta dal semaforo ad indice 2 in maniera tale che sia
	 * impostato al valore corretto durante la normale esecuzione
	 * del protocollo di sincronizzazione contenuta nel ciclo while.
	 */
	buf[0].sem_num = 2;
	buf[0].sem_op = -1;
	buf[0].sem_flg = 0;

	semop(semid, buf, 1);

	while (1)
	{
		/*
		 * Attesa del Lettore sul semaforo ad indice 1, in attesa del
		 * completamento delle attività dello Scrittore che segnalerà
		 * il suo completamento su questo semaforo.
		 */
		buf[0].sem_num = 1;
		buf[0].sem_op = -1;
		buf[0].sem_flg = 0;

		semop(semid, buf, 1);

		printf("Il Lettore con PID %d ha letto il messaggio: %s\n", getpid(), (char *) message);

		/*
		 * Ogni lettore segnala il completamento della sua attività di
		 * lettura allo Scrittore per mezzo del semaforo ad indice 0,
		 * il quale verrà sbloccato solo al completamento di tutte
		 * le letture.
		 */
		buf[0].sem_num = 0;
		buf[0].sem_op = 1;
		buf[0].sem_flg = 0;

		semop(semid, buf, 1);

		/*
		 * Questa barriera, imposta ai Lettori per mezzo del semaforo ad
		 * indice 2, impedisce ad uno stesso lettore di effettuare molteplici
		 * letture dello stesso messaggio consumando più volte il gettone
		 * destinato ad altri lettori. Sarà lo Scrittore a sbloccare tutti
		 * i lettori bloccati su questo semaforo non appena avrà garanzia
		 * che ognuno di essi abbia completato.
		 */
		buf[0].sem_num = 2;
		buf[0].sem_op = -1;
		buf[0].sem_flg = 0;

		semop(semid, buf, 1);
	}
}

void writer_function(void)
{
	struct sembuf buf[1];

	/*
	 * Lo Scrittore si mette in attesa del completamento delle
	 * operazioni di lettura di ogni singolo Lettore.
	 */
	buf[0].sem_num = 0;
	buf[0].sem_op = -N_READER;
	buf[0].sem_flg = 0;

	semop(semid, buf, 1);

	/*
	 * Sblocca i lettori dalla barriera imposta dal semaforo con
	 * indice 2 al fine di riabilitare i lettori per la lettura di
	 * un nuovo messaggio che lo Scrittore andrà a scrivere a breve.
	 * In questo modo ogni lettore ha la possibilità di leggere ogni
	 * nuovo messaggio, una ed una sola volta.
	 */
	buf[0].sem_num = 2;
	buf[0].sem_op = N_READER;
	buf[0].sem_flg = 0;

	semop(semid, buf, 1);

	printf("\nLo Scrittore con PID %d chiede di inserire un messaggio: ", getpid());
	if (scanf("%[^\n]", message) == 0)
		(*message) = '\0';
	getc(stdin);

	/*
	 * Qui segnaliamo a tutti i Lettori, per mezzo del semaforo ad indice 1,
	 * che lo Scrittore ha completato le attività di scrittura di un nuovo
	 * messaggio.
	 */
	buf[0].sem_num = 1;
	buf[0].sem_op = N_READER;
	buf[0].sem_flg = 0;

	semop(semid, buf, 1);
}

int main()
{
	int i;
	int ret;

	pid_t pid;
	pid_t pid_reader[N_READER];

	union semun sem_arg;

	struct sembuf buf[1];

	ret = 1;

	/*
	 * Setup della memoria condivisa utilizzata dai
	 * Lettori e dagli Scrittori per leggere e scrivere
	 * i messaggi.
	 */
	if ((msgid = shmget(SHMEM_MSG_KEY, SHMEM_MSG_SIZE, IPC_CREAT|IPC_EXCL|0666)) == -1)
		goto exit_process_4;
	if ((message = (char *) shmat(msgid, NULL, 0)) == (void *) -1)
		goto exit_process_3;

	/*
	 * Setup di 3 istanze di Semaforo con i
	 * seguenti indici:
	 *  0 - Semaforo per lo Scrittore inizializzato
	 *      ad N_READER gettoni;
	 *  1 - Semaforo per tutti i Lettori inizializzato
	 *      a 0;
	 *  2 - Semaforo per tutti i Lettori che che hanno
	 *      completato la singola operazione di lettura
	 *      ammessa per ognuno di essi ed inizializzato
	 *      a 0.
	 */
	if ((semid = semget(SEMAPHORE_KEY, 3, IPC_CREAT|0666)) == -1)
		goto exit_process_2;
	sem_arg.val = N_READER;
	if (semctl(semid, 0, SETVAL, sem_arg) == -1)
		goto exit_process_1;
	sem_arg.val = 0;
	if (semctl(semid, 1, SETVAL, sem_arg) == -1)
		goto exit_process_1;
	sem_arg.val = 0;
	if (semctl(semid, 2, SETVAL, sem_arg) == -1)
		goto exit_process_1;

	/* 
	 * Generazione dei processi Lettori.
	 */
	for (i=0; i<N_READER; i++)
	{
		if ((pid = fork()) == -1)
			goto exit_process_1;
		else if (pid == 0)
			reader_function();
		else
			pid_reader[i] = pid;
	}

	for (i=0; i<N_STEP; i++)
		writer_function();

	/*
	 * Attendiamo che gli ultimi lettori abbiano terminato
	 * la loro attività.
	 */
	buf[0].sem_num = 0;
	buf[0].sem_op = -N_READER;
	buf[0].sem_flg = 0;

	semop(semid, buf, 1);

	/*
	 * Uccidiamo i processi lettori prima di terminare.
	 */
	for (i=0; i<N_READER; i++)
		kill(pid_reader[i], SIGKILL);

	printf("\n");

	ret = 0;

exit_process_1:
	semctl(semid, -1, IPC_RMID, sem_arg);
exit_process_2:
	shmdt(message);
exit_process_3:
	shmctl(msgid, IPC_RMID, NULL);
exit_process_4:
	exit(ret);
}