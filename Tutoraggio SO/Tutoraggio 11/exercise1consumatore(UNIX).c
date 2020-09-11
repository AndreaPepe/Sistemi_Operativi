#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>

#define SHMEMSIZE 4096

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};

int consumatore(void* addr, int ds)
{
	struct sembuf buf[1];

	buf[0].sem_num = 0;
	buf[0].sem_op = -1;
	buf[0].sem_flg = 0;

	/* WAIT sulla prima istanza di Semaforo */
	if (semop(ds, buf, 1) == -1) {
		printf("Errore. Impossibile sincronizzare sul semaforo.\n");
		return 0;
	}

	printf("Il produttore scrive: %s\n", (char *) addr);

	buf[0].sem_num = 1;
	buf[0].sem_op = 1;
	buf[0].sem_flg = 0;

	/* SIGNAL sulla seconda istanza di Semaforo */
	if (semop(ds, buf, 1) == -1) {
		printf("Errore. Impossibile sincronizzare sul semaforo.\n");
		return 0;
	}

	return 1;
}

int main()
{
	int ret;
	int shm_ds, sem_ds;
	key_t shm_key, sem_key;
	void *shm_addr;

	union semun sem_arg;

	shm_key = 6868;
	sem_key = 3232;

	if ((shm_ds = shmget(shm_key, SHMEMSIZE, IPC_CREAT|0666)) == -1) {
		printf("Errore. Impossibile allocare la memoria condivisa.\n");
		return 1;
	}

	if ((shm_addr = shmat(shm_ds, NULL, 0)) == (void *) -1) {
		printf("Errore. Impossibile agganciare la memoria condivisa.\n");
		shmctl(shm_ds, IPC_RMID, NULL);
		return -1;
	}

	/* Creazione di 2 istanze dell'oggetto Semaforo */
	if ((sem_ds = semget(sem_key, 2, IPC_CREAT|0666)) == -1) {
		printf("Errore. Impossibile allocare il semaforo.\n");
		shmctl(shm_ds, IPC_RMID, NULL);
		shmdt(shm_addr);
		return 1;
	}

	sem_arg.val = 0;

	/* Imposta a 0 il valore della prima istanza dell'oggetto Semaforo */
	if (semctl(sem_ds, 0, SETVAL, sem_arg) == -1) {
		printf("Errore. Impossibile inizializzare il semaforo.\n");
		semctl(sem_ds, -1, IPC_RMID, sem_arg);
		shmctl(shm_ds, IPC_RMID, NULL);
		shmdt(shm_addr);
		return 1;
	}

	sem_arg.val = 1;

	/* Imposta a 1 il valore della seconda istanza dell'oggetto Semaforo */
	if (semctl(sem_ds, 1, SETVAL, sem_arg) == -1) {
		printf("Errore. Impossibile inizializzare il semaforo.\n");
		semctl(sem_ds, -1, IPC_RMID, sem_arg);
		shmctl(shm_ds, IPC_RMID, NULL);
		shmdt(shm_addr);
		return 1;
	}

	do {
		ret = consumatore(shm_addr, sem_ds);
	} while (ret);

	semctl(sem_ds, -1, IPC_RMID, sem_arg);
	shmctl(shm_ds, IPC_RMID, NULL);
	shmdt(shm_addr);

	return 0;
}