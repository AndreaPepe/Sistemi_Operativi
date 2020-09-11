#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>


typedef struct __attribute__((packed)) __packet {
	struct __packet *next_packet;
	unsigned long int sender;
	unsigned long int recipient;
	char message[256];
} packet;

typedef struct __fifo_queue {
	sem_t semaphore __attribute__((aligned(64)));
	pthread_spinlock_t lock __attribute__((aligned(64)));
	struct __packet *head;
	struct __packet *tail;
} fifo_queue;


unsigned long int num_threads;

fifo_queue master_fifo_queue;
fifo_queue *thread_fifo_queue;


void * thread_function(void * arg)
{
	packet *pkt;
	unsigned long int num = 0;
	unsigned long int id = *((unsigned long int*)&arg);

	while (1)
	{
		if (sem_trywait(&thread_fifo_queue[id].semaphore) == 0)
		{
			pthread_spin_lock(&thread_fifo_queue[id].lock);

			pkt = thread_fifo_queue[id].head;

			if ((thread_fifo_queue[id].head = pkt->next_packet) == NULL)
				thread_fifo_queue[id].tail = NULL;

			pthread_spin_unlock(&thread_fifo_queue[id].lock);

			printf("Thread %lu has received a message from %lu: %s\n", id, pkt->sender, pkt->message);
			fflush(stdout);

			free((void *) pkt);
		}
		else
		{
			if ((pkt = (packet *) malloc(sizeof(packet))) == NULL)
				continue;

			pkt->next_packet = NULL;

			pkt->sender = id;
			pkt->recipient = rand() % num_threads;

			if (snprintf(pkt->message, 256, "this is the message number %lu", ++num) >= 256)
				pkt->message[255] = '\0';

			pthread_spin_lock(&master_fifo_queue.lock);

			if (master_fifo_queue.tail != NULL)
				master_fifo_queue.tail->next_packet = pkt;
			else
				master_fifo_queue.head = pkt;

			master_fifo_queue.tail = pkt;

			pthread_spin_unlock(&master_fifo_queue.lock);

			sem_post(&master_fifo_queue.semaphore);

			usleep(100000);
		}
	}

	pthread_exit(NULL);
}

void master_function(void)
{
	packet *pkt;
	unsigned long int recipient;

	while (1)
	{
		sem_wait(&master_fifo_queue.semaphore);

		pthread_spin_lock(&master_fifo_queue.lock);

		pkt = master_fifo_queue.head;

		if ((master_fifo_queue.head = pkt->next_packet) == NULL)
			master_fifo_queue.tail = NULL;

		pthread_spin_unlock(&master_fifo_queue.lock);

		pkt->next_packet = NULL;
		recipient = pkt->recipient;

		pthread_spin_lock(&thread_fifo_queue[recipient].lock);

		if (thread_fifo_queue[recipient].tail != NULL)
			thread_fifo_queue[recipient].tail->next_packet = pkt;
		else
			thread_fifo_queue[recipient].head = pkt;

		thread_fifo_queue[recipient].tail = pkt;

		pthread_spin_unlock(&thread_fifo_queue[recipient].lock);

		sem_post(&thread_fifo_queue[recipient].semaphore);
	}
}

int main(int argc, char *argv[])
{
	unsigned long int i;

	if (argc < 2)
		return -1;

	num_threads = strtoul(argv[1], NULL, 10);

	if ((thread_fifo_queue = (fifo_queue *) calloc(num_threads, sizeof(fifo_queue))) == NULL)
		return -1;

	pthread_t threads[num_threads];

	sem_init(&master_fifo_queue.semaphore, 0, 0);
	pthread_spin_init(&master_fifo_queue.lock, PTHREAD_PROCESS_PRIVATE);

	for (i=0; i<num_threads; i++)
	{
		sem_init(&thread_fifo_queue[i].semaphore, 0, 0);
		pthread_spin_init(&thread_fifo_queue[i].lock, PTHREAD_PROCESS_PRIVATE);
		pthread_create(&threads[i], NULL, thread_function, *((void**)&i));
	}

	master_function();

	for (i=0; i<num_threads; i++)
	{
		pthread_join(threads[i], NULL);
		pthread_spin_destroy(&thread_fifo_queue[i].lock);
		sem_destroy(&thread_fifo_queue[i].semaphore);
	}

	return 0;
}