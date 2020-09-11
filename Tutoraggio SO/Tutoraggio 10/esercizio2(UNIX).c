#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SHMEMSIZE 4096

void produttore(key_t key)
{
	int ret;
	void *addr;

	/* Returns the identifier of the System V shared memory segment associated with the
	   value of the argument "key".  Here we require the shared memory does not alredy
	   exist since we want this memory is exclusively created and associated to the
	   passed argument "key".  A new shared memory segment with size equal to the value
	   of "size" rounded up to a multiple of PAGE_SIZE is created.  When a new shared
	   memory segment is created, its contents are initialized to zero values, and its
	   associated data structure "shmid_ds" is properly initialized.  */
	if ((ret = shmget(key, SHMEMSIZE, IPC_CREAT|IPC_EXCL|0666)) == -1) {
		printf("Errore del produttore. Impossibile allocare la memoria condivisa.\n");
		exit(1);
	}

	/* Attaches the System V shared memory segment identified by "shmid" to the address
	   space of the calling process.  Since "shmaddr" is NULL, the system chooses a
	   suitable (unused) page-aligned address to attach the segment.  On success, the
	   aforementioned address is returned by the function.  */
	if ((addr = shmat(ret, NULL, 0)) == (void *) -1) {
		printf("Errore del produttore. Impossibile agganciare la memoria condivisa.\n");
		shmctl(ret, IPC_RMID, NULL);
		exit(1);
	}

	printf("\nInserisci una stringa da condividere con il consumatore:  ");
	scanf("%[^\n]", (char *) addr);

	/* Detaches the shared memory segment located at the address specified by shmaddr
	   from the address space of the calling process.  On a successful call, the system
	   updates the members of the "shmid_ds" structure associated with the shared memory
	   segment, and in particular the "shm_nattch" value is decremented by 1.  */
	shmdt(addr);
}

void consumatore(key_t key)
{
	int ret;
	void *addr;

	/* Returns the identifier of the System V shared memory segment associated with the
	   value of the argument "key".  Here we are indicating the shared memory already
	   exists and we want to get back a correct identifier to perfotm operations on the
	   shared memory.  */
	if ((ret = shmget(key, SHMEMSIZE, 0)) == -1) {
		printf("Errore del consumatore. Impossibile reperire la memoria condivisa.\n");
		exit(1);
	}

	/* Attaches the System V shared memory segment identified by "shmid" to the address
	   space of the calling process.  Since "shmaddr" is NULL, the system chooses a
	   suitable (unused) page-aligned address to attach the segment.  On success, the
	   aforementioned address is returned by the function.  */
	if ((addr = shmat(ret, NULL, 0)) == (void *) -1) {
		printf("Errore del consumatore. Impossibile agganciare la memoria condivisa.\n");
		shmctl(ret, IPC_RMID, NULL);
		exit(1);
	}

	printf("\nIl messaggio del produttore è:  %s\n\n", (char *) addr);

	/* Performs the control operation specified by cmd on the System V shared memory
	   segment whose identifier is given in "shmid".  Here we indicate to mark the segment
	   in order to be destroyed as soon as the number of attached processes becomes zero.  */
	shmctl(ret, IPC_RMID, NULL);

	/* Detaches the shared memory segment located at the address specified by shmaddr
	   from the address space of the calling process.  On a successful call, the system
	   updates the members of the "shmid_ds" structure associated with the shared memory
	   segment, and in particular the "shm_nattch" value is decremented by 1.  Shared
	   memory can now be removed by the operating system since no processes are attached
	   to it.  */
	shmdt(addr);
}

int main()
{
	key_t key;
	pid_t pid;
	int wstatus;

	key = 6832;

	if ((pid = fork()) == -1) {
		printf("Errore. Impossibile creare il processo figlio.\n");
		return -1;
	}

	if (pid) {
		waitpid(pid, &wstatus, 0);
		if (!WIFEXITED(wstatus)) {
			printf("Errore. Terminazione scorretta processo figlio.\n");
			return -1;
		} else if (WEXITSTATUS(wstatus) == 1) {
			printf("Errore. Terminazione processo figlio con stato %d\n", WEXITSTATUS(wstatus));
			return -1;
		}
		consumatore(key);
	} else {
		produttore(key);
	}

	return 0;
}