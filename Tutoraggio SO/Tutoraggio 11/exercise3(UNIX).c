#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMBER_THREADS	64
#define ALLOWED_THREADS	4

#define STRING_MAX_LENGTH	48

pthread_barrier_t barrier;

sem_t semaphore;
pthread_spinlock_t lock;

char *string_buffer;
long long string_index;

long long num_ops = 1000000;

void * thread_funtion(void *arg)
{
	int loop = 1;

	long long thread_msg = 0;
	int thread_id = *((int *)&arg);

	long long local_index;
	char local_buffer[STRING_MAX_LENGTH];

	pthread_barrier_wait(&barrier);

	while (loop)
	{
		sem_wait(&semaphore);
		pthread_spin_lock(&lock);

		if (num_ops > 0)
		{
			local_index = snprintf(local_buffer, STRING_MAX_LENGTH, "Message %lli of Thread %d\n", ++thread_msg, thread_id);

			strncpy(&string_buffer[string_index], local_buffer, STRING_MAX_LENGTH);

			string_index += local_index;

			num_ops--;
		}
		
		if (num_ops == 0)
		{
			loop = 0;
		}

		pthread_spin_unlock(&lock);
		sem_post(&semaphore);
	}

	pthread_exit(NULL);
}

int main()
{
	int i;
	pthread_t threads[NUMBER_THREADS];

	long nanosecs;
	struct timespec start;
	struct timespec end;

	string_index = 0;
	string_buffer = malloc((size_t) num_ops * STRING_MAX_LENGTH);

	pthread_barrier_init(&barrier, NULL, NUMBER_THREADS+1);

	sem_init(&semaphore, 0, ALLOWED_THREADS);
	pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);

	for (i=0; i<NUMBER_THREADS; i++)
		if (pthread_create(&threads[i], NULL, thread_funtion, *((void **)&i)))
			return -1;

	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	pthread_barrier_wait(&barrier);

	for (i=0; i<NUMBER_THREADS; i++)
		pthread_join(threads[i], NULL);

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);

	pthread_spin_destroy(&lock);
	sem_destroy(&semaphore);

	pthread_barrier_destroy(&barrier);

	nanosecs = ((long) end.tv_sec * 1000000000L) + end.tv_nsec;
	nanosecs -= ((long) start.tv_sec * 1000000000L) + start.tv_nsec;

	printf("%li\n", nanosecs);

	// printf("%s", string_buffer);

	free(string_buffer);

	return 0;
}