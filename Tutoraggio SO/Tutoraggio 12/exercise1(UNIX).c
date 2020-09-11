#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>


typedef struct __attribute__((packed)) __packet {
	struct __packet *next_packet;
	unsigned long int source;
	char message[256];
} packet;


unsigned long int num_queues;
unsigned long int *len_queue;

pthread_spinlock_t *queue_lock;

packet **queues;


void * producer_function(void *arg)
{
	packet *pkt;
	unsigned long int i;
	int id = *((int*)&arg);
	unsigned long int num = 0;

	while (1)
	{
		unsigned long int min_len = ~(0UL);
		unsigned long int min_len_id = 0;

		usleep(100000);

		if ((pkt = (packet *) malloc(sizeof(packet))) == NULL)
			continue;

		pkt->source = id;

		if (snprintf(pkt->message, 256, "this is the message number %lu", ++num) >= 256)
			pkt->message[255] = '\0';

		do
		{
			for (i=0; i<num_queues; i++)
			{
				if (len_queue[i] < min_len)
				{
					min_len_id = i;
					min_len = __atomic_load_n (&len_queue[i], __ATOMIC_RELAXED);
				}
			}
		}
		while (pthread_spin_trylock(&queue_lock[min_len_id]));

		__atomic_store_n (&len_queue[min_len_id], len_queue[min_len_id] + 1, __ATOMIC_RELAXED);
		pkt->next_packet = queues[min_len_id];
		queues[min_len_id] = pkt;

		pthread_spin_unlock(&queue_lock[min_len_id]);
	}

	pthread_exit(NULL);
}

void * consumer_function(void *arg)
{
	packet *pkt;
	int id = *((int*)&arg);

	while (1)
	{
		if (len_queue[id] == 0)
		{
			usleep(100000);
			continue;
		}

		pthread_spin_lock(&queue_lock[id]);

		pkt = queues[id];
		queues[id] = pkt->next_packet;
		__atomic_store_n (&len_queue[id], len_queue[id] - 1, __ATOMIC_RELAXED);

		pthread_spin_unlock(&queue_lock[id]);

		printf("Message from Producer %lu: %s\n", pkt->source, pkt->message);

		free((void *) pkt);
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int i;

	unsigned long int num_producers;
	unsigned long int num_consumers;

	if (argc < 3)
		return -1;

	num_producers = strtoul(argv[1], NULL, 10);
	num_consumers = strtoul(argv[2], NULL, 10);

	num_queues = num_consumers;

	pthread_t producers[num_producers];
	pthread_t consumers[num_consumers];

	if ((len_queue = (unsigned long int *) calloc(num_queues, sizeof(unsigned long int))) == NULL)
		return -1;

	if ((queue_lock = (pthread_spinlock_t *) calloc(num_queues, sizeof(pthread_spinlock_t))) == NULL)
		return -1;

	if ((queues = (packet **) calloc(num_queues, sizeof(packet *))) == NULL)
		return -1;

	for (i=0; i<num_consumers; i++)
	{
		pthread_spin_init(&queue_lock[i], PTHREAD_PROCESS_PRIVATE);
		pthread_create(&consumers[i], NULL, consumer_function, *((void**)&i));
	}

	for (i=0; i<num_producers; i++)
		pthread_create(&producers[i], NULL, producer_function, *((void**)&i));

	for (i=0; i<num_producers; i++)
		pthread_join(producers[i], NULL);

	for (i=0; i<num_consumers; i++)
	{
		pthread_join(consumers[i], NULL);
		pthread_spin_destroy(&queue_lock[i]);
	}

	free((void *) queues);
	free((void *) len_queue);

	return 0;
}