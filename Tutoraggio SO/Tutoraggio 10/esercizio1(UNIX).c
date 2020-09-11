#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 4

void * extend_message(void *arg)
{
	size_t local_length;
	size_t message_length = 0;

	char *tmp;
	char *local;
	char *message = (char *) arg;

	printf("Inserisci un messaggio: ");

	if (scanf("%m[^\n]", &local) > 0)
	{
		getchar();

		if ((local_length = strlen(local)) > 0)
		{
			if (message != NULL)
				message_length = strlen(message);

			if ((tmp = (char *) malloc(message_length + local_length + 2)) != NULL)
			{
				if (message != NULL)
					strncpy(tmp, message, message_length);

				strncpy(&tmp[message_length], local, local_length);
				tmp[message_length+local_length] = ' ';
				tmp[message_length+local_length+1] = '\0';

				if (message != NULL)
					free((void *) message);

				message = tmp;
			}
		}

		free((void *) local);
	}

	pthread_exit((void *) message);
}

int main()
{
	int i;
	char *message;
	pthread_t thread;

	for (i=0, message=NULL; i<NUM_THREADS; i++)
		if (pthread_create(&thread, NULL, extend_message, (void *) message) == 0)
			pthread_join(thread, (void **) &message);

	printf("Tutti i messaggi: %s\n", message);

	free((void *) message);

	return 0;
}