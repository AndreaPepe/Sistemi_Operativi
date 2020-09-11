#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define SIZE 10000000

#ifndef MUTEX
static pthread_spinlock_t lock;
#else
static pthread_mutex_t mutex;
#endif
static unsigned int data[SIZE];
static unsigned int index = 0;

void * thread_function(void *arg)
{
	while (1) {
#ifndef MUTEX
		if (pthread_spin_lock(&lock) == EDEADLOCK)
#else
		if (pthread_mutex_lock(&mutex))
#endif
		{
			pthread_exit((void *) 1);
		}
		if (index < SIZE){
			data[index] = index;
			index++;
		} else {
#ifndef MUTEX
			pthread_spin_unlock(&lock);
#else
			pthread_mutex_unlock(&mutex);
#endif
			break;
		}
#ifndef MUTEX
		pthread_spin_unlock(&lock);
#else
		pthread_mutex_unlock(&mutex);
#endif
	}
	pthread_exit((void *) 0);
}

int check_correctness()
{
	unsigned int i;
	for (i=0; i<SIZE; i++) {
		if (data[i] != i)
			return 1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	void *res;
	pthread_t *threads;
	unsigned short int t, max_threads, cur_threads = 0;

	if (argc < 2) {
		do {
			printf("Inserisci il numero di Threads da utilizzare: ");
			scanf("%hu", &max_threads);
			if (max_threads >= 2)
				break;
			printf("Attenzione. Servono almeno 2 Threads. ");
		} while (1);
	} else {
		if ((max_threads = (unsigned int) atoi(argv[1])) < 2) {
			printf("Errore. Servono almeno 2 Threads.\n");
			return 1;
		}
	}

	if ((threads = malloc(max_threads*sizeof(pthread_t))) == NULL) {
		printf("Errore. Non è stato possibile allocare %hu pthread_t strutture dati.\n", max_threads);
		return 1;
	}

#ifndef MUTEX
	if (pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE)) {
		printf("Errore. Impossibile inizializzare il lock.\n");
		free(threads);
		return 1;
	}
#else
	if(pthread_mutex_init(&mutex, NULL)) {
		printf("Errore. Impossibile inizializzare il mutex.\n");
		free(threads);
		return 1;
	}
#endif

	for (t=0; t<max_threads; t++) {
		switch (pthread_create(&threads[cur_threads], NULL, thread_function, NULL)) {
			case 0:
				cur_threads++;
				break;
			default:
				printf("Attenzione. Non è stato possibile generare un nuovo Thread.\n");
				break;
		}
	}

	printf("Sono stati generati %hu nuovi Threads.\n", cur_threads);

	for (t=0; t<cur_threads; t++) {
		switch(pthread_join(threads[t], &res)) {
			case 0:
				if (res)
					printf("Il Thread ha terminato con valore di uscita %llu.\n", (unsigned long long int) res);
				break;
			case EDEADLK:
				printf("Attenzione. E' stata riscontrata una condizione di deadlock.\n");
				break;
			case EINVAL:
				printf("Attenzione. Il Thread che si sta attendendo non è joinable.\n");
				break;
			case ESRCH:
				printf("Attenzione. Nessun Thread è associato al valore pthread_t passato.\n");
				break;
		}
	}

#ifndef MUTEX
	pthread_spin_destroy(&lock);
#else
	pthread_mutex_destroy(&mutex);
#endif
	free(threads);

	if (check_correctness()) {
		printf("Errore. L'esecuzione ha deviato dalle specifiche di programma.\n");
		return 1;
	} else {
		printf("Programma eseguito correttamente.\n");
	}

	return 0;
}