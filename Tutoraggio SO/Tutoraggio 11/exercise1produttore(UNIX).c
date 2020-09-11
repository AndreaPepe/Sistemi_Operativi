#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>

#define SHMEMSIZE 4096

int produttore(void* addr, int ds)
{
	struct sembuf buf[1];

	buf[0].sem_num = 1;
	buf[0].sem_op = -1;
	buf[0].sem_flg = 0;

	/* WAIT sulla seconda istanza di Semaforo */
	if (semop(ds, buf, 1) == -1) {
		printf("Errore. Impossibile sincronizzare sul semaforo.\n");
		return 0;
	}

	printf("Scrivi al consumatore: ");
	if (scanf("%[^\n]", (char *) addr) == 0)
		*((char *) addr) = '\0';
	getc(stdin);

	buf[0].sem_num = 0;
	buf[0].sem_op = 1;
	buf[0].sem_flg = 0;

	/* SIGNAL sulla prima istanza di Semaforo */
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

	shm_key = 6868;
	sem_key = 3232;

	if ((shm_ds = shmget(shm_key, SHMEMSIZE, 0)) == -1) {
		printf("Errore. Impossibile reperire la memoria condivisa.\n");
		return 1;
	}

	if ((shm_addr = shmat(shm_ds, NULL, 0)) == (void *) -1) {
		printf("Errore. Impossibile agganciare la memoria condivisa.\n");
		return -1;
	}

	if ((sem_ds = semget(sem_key, 2, 0)) == -1) {
		printf("Errore. Impossibile reperire il semaforo.\n");
		shmdt(shm_addr);
		return 1;
	}

	do {
		ret = produttore(shm_addr, sem_ds);
	} while (ret);

	shmdt(shm_addr);

	return 0;
}