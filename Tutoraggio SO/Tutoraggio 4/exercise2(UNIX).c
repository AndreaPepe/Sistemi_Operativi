#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


void *foo1(void *arg)
{
	char *buff;

	printf("\nInserisci un messaggio: ");
	scanf("%m[^\n]", &buff);

	pthread_exit((void *) buff);
}


void *foo2(void *arg)
{
	char *buff;

	if ((buff = (char *) arg) == NULL)
		pthread_exit((void *) 1);

	printf("Messaggio letto: %s\n\n", buff);

	pthread_exit((void *) 0);
}


int main()
{
	void *buff;
	void *exit_status;
	pthread_t thread;

	if (pthread_create(&thread, NULL, foo1, NULL))
	{
		printf("Unable to create thread for executing foo1. Exit.\n");
		return 1;
	}

	if (pthread_join(thread, &buff))		// scrive in buff ciò che l'utente scrive e lo ritorna come codice di uscita
	{
		printf("Unable to join thread executing foo1. Exit.\n");
		return 1;
	}

	if (pthread_create(&thread, NULL, foo2, buff))  // riceve in input il CODICE DI USCITA di foo1
	{
		printf("Unable to create thread for executing foo2. Exit.\n");
		return 1;
	}

	if (pthread_join(thread, &exit_status))
	{
		printf("Unable to join thread executing foo2. Exit.\n");
		return 1;
	}

	free(buff);
	
	return 0;
}
