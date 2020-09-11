#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void ring_node(int proc_id, int prev_id, int next_id)
{
	int num;

	int *int_p;
	int *int_n;

	void *addr_p;
	void *addr_n;

	if ((addr_p = shmat(prev_id, NULL, 0)) == (void *) -1)
		return;

	int_p = (int *) addr_p;

	if ((addr_n = shmat(next_id, NULL, 0)) == (void *) -1)
	{
		shmdt(addr_p);
		return;
	}

	int_n = (int *) addr_n;

	num = 0;

	while (num < 30)
	{
		while (*int_p == num)
			sleep(1);

		num = *int_p;

		printf("Process %d has read number %d\n", proc_id, num);
		fflush(stdout);

		sleep(1);

		*int_n = num + 1;
	}

	shmdt(addr_n);
	shmdt(addr_p);
}

int main(int argc, char *argv[])
{
	pid_t pid;
	key_t key;
	int i, j, nproc, wstatus;

	if (argc < 2)
		return -1;

	nproc = atoi(argv[1]);

	if (nproc < 2)
		return -1;

	int shmid[nproc][2];

	for (i=0, key=6832; i<nproc; i++)
	{
		if (i == 0)
		{
			if ((shmid[i][0] = shmget(key++, 4096, IPC_CREAT|IPC_EXCL|0666)) == -1)
				return -1;
		}
		else
		{
			shmid[i][0] = shmid[i-1][1];
		}

		if (i < (nproc - 1))
		{
			if ((shmid[i][1] = shmget(key++, 4096, IPC_CREAT|IPC_EXCL|0666)) == -1)
			{
				shmctl(shmid[0][0], IPC_RMID, NULL);
				for (j=0; j<i; j++)
					shmctl(shmid[j][1], IPC_RMID, NULL);
				return -1;
			}
		}
		else
		{
			shmid[i][1] = shmid[0][0];
		}

		if ((pid = fork()) == -1)
		{
			shmctl(shmid[0][0], IPC_RMID, NULL);
			for (j=0; j<=i; j++)
				if (j < (nproc - 1))
					shmctl(shmid[j][1], IPC_RMID, NULL);
			return -1;
		}
		else if (pid == 0)
		{
			ring_node(i, shmid[i][0], shmid[i][1]);
			return 0;
		}
	}

	void *addr = shmat(shmid[0][0], NULL, 0);
	*((int *) addr) = 1;
	shmdt(addr);

	for (i=0; i<nproc; i++)
		wait(&wstatus);

	for (i=0; i<nproc; i++)
		shmctl(shmid[i][1], IPC_RMID, NULL);

	return 0;
}