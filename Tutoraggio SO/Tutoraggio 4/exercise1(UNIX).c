#include <unistd.h>
#include <pthread.h>
#include <stdio.h>


/*
 * > https://en.wikipedia.org/wiki/Volatile_(computer_programming)
 * La keyword "volatile" indica al compilatore che il valore di una
 * variabile può cambiare tra due o più accessi differenti alla stessa
 * variabile, anche se questa non risulta essere modificata da alcuna
 * istruzione su quel flusso di controllo. Previene dunque che un
 * compilatore, nel apportare ottimizzazioni al codice, riusi un valore
 * "stale" in accessi consecutivi a quella variabile.
 *
 * > https://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Variable-Attributes.html
 * La keyword "__attribute__ ((aligned(#)))" permette di specificare l'allineamento
 * preciso di una variabile in memoria per sfruttare al massimo una specifica
 * architettura. E' utile al fine di garantire che la memoria dove il contenuto
 * di una variabile alloggia non ricada in due linee di cache differenti.
 */
volatile unsigned int shared_variable __attribute__ ((aligned(4))) = 1;


void *thread_function(void *arg)
{
	printf("2. Child Thread is executing.\n");
	sleep(5);
	printf("3. Child Thread is terminating.\n");
	shared_variable = 0;
	pthread_exit(NULL);
}


int main()
{
	int result;
	// void *exit_status;
	pthread_t thread;
	printf("1. Parent Thread is executing.\n");
	if ((result = pthread_create(&thread, NULL, thread_function, NULL))) {
		printf("Unable to create a new thread. Exit.\n");
		return 1;
	} else {
		while (shared_variable) ;
		printf("4. Parent Thread is terminating.\n");
		// pthread_join(thread, &exit_status);
	}
	return 0;
}
