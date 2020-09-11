#include <stdio.h>
#include <string.h>
#include <pthread.h>


#define N 10


typedef struct __message_node {
	/*
	 * Anche un singolo attributo all'interno
	 * di una struttura può essere dichiarato
	 * come "volatile".
	 */
	volatile unsigned available;
	char message[512];
} message_node;


message_node message_array[N];


void *message_passing(void *arg)
{
	int i = *((int *)&arg);

	while (message_array[i].available == 0) ;

	printf("Thread %d ha ricevuto il messaggio: %s\n", i, message_array[i].message);

	if (i < (N-1))
	{
		printf("Inserisci un messaggio per il thread %d: ", (i+1));
		scanf("%[^\n]", message_array[(i+1)].message);
		while (getchar() != (int) '\n') ;
		message_array[(i+1)].available = 1;
	}

	pthread_exit(0);
}


int main()
{
	int i;
	pthread_t thread[N];

	/*
	 * Lo standard C89 garantisce che gli array e
	 * le variabili globali siano inizializzati a 0.
	 * La funzione "memset" poteva essere omessa.
	 */
	memset((void *) message_array, 0, N * sizeof(message_node));

	for (i=0; i<N; i++)
		if (pthread_create(&thread[i], NULL, message_passing, *((void **)&i)))
			return 1;

	printf("Inserisci un messaggio per il thread 0: ");
	scanf("%[^\n]", message_array[0].message);
	while (getchar() != (int) '\n') ;
	message_array[0].available = 1;

	for (i=0; i<N; i++)
		pthread_join(thread[i], NULL);

	return 0;
}
