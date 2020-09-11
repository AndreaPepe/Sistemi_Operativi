#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

pthread_barrier_t barrier1;
pthread_barrier_t barrier2;

void * thread_function(void *arg)
{
	int i;
	char buffer[256];

	printf("Inserisci una stringa: ");
	scanf("%[^\n]", buffer);

	/*
	 * Sblocca il processo padre dalla prima barriera
	 * che gli impediva di procedere nella lettura del
	 * buffer conentenete lo stack di questo processo
	 * prima che la stringa reperita con "scanf" fosse
	 * stata realmente salvata in un buffer nello stack.
	 */
	pthread_barrier_wait(&barrier1);

	/*
	 * Questa barriera tiene in attesa il thread per la
	 * terminazione della lettura del buffer contenente
	 * lo stack di questo thread da parte del processo
	 * padre. Questo per prevenire che l'invocazione di
	 * "pthread_exit" sovrascriva il contenuto del buffer.
	 */
	pthread_barrier_wait(&barrier2);

	pthread_exit(NULL);
}

int main()
{
	pthread_attr_t attr;
	pthread_t thrd;
	int i;

	/* 16384 è PTHREAD_STACK_MIN */
	char thread_stack[16384];
	memset((void *) thread_stack, (int) ' ', 16384);

	pthread_barrier_init(&barrier1, NULL, 2);
	pthread_barrier_init(&barrier2, NULL, 2);

	if (pthread_attr_init(&attr)) {
		printf("Error. Unable to initialize thread attributes.\n");
		return 1;
	}

	/*
	 * Creiamo un nuovo thread che allocherà un buffer per
	 * contenere una stringa sullo stack. Tale stack vivrà
	 * all'interno di un buffer allocato sullo stack del
	 * processo padre.
	 *
	 *   "Noi creiamo il mondo del sogno,
	 *    portiamo il soggetto dentro quel
	 *    sogno e lui lo riempie con i sui
	 *    segreti."
	 *
	 *    (cit. Leonardo Di Caprio, Inception)
	 *
	 * Lo facciamo impostando l'indirizzo e la dimensione
	 * dello stack negli attribbuti di creazione del thread.
	 */
	if (pthread_attr_setstack(&attr, (void *) thread_stack, 16384)) {
		printf("Warning. Unable to set thread stack size.\n");
	}

	if (pthread_create(&thrd, &attr, thread_function, NULL)) {
		printf("Error. Unable to start new thread.\n");
		return 1;
	}

	if (pthread_attr_destroy(&attr)) {
		printf("Warning. Unable to destroy thread attributes.\n");
	}

	pthread_barrier_wait(&barrier1);

	for (i=0; i<16384; i++)
	{
		putchar((int) thread_stack[i]);
	}

	pthread_barrier_wait(&barrier2);

	if (pthread_join(thrd, NULL)) {
		printf("Error. Unable to join created thread.\n");
		return 1;
	}

	pthread_barrier_destroy(&barrier1);
	pthread_barrier_destroy(&barrier2);

	return 0;
}
