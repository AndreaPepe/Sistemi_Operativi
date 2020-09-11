#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STEPS 4

void master_function(FILE *filp[], int ns)
{
	int i, s;
	char *message;

	for (s=0; s<STEPS; s++)
	{
		printf("[MASTER] Insert a message to broadcast: ");
		scanf("%m[^\n]", &message);
		getchar();

		for (i=0; i<ns; i++)
		{
			if (filp[i] != NULL)
			{
				fprintf(filp[i], "%s\n", message);
				fflush(filp[i]);
			}
		}

		free(message);

		sleep(1);
	}
}

int main(int argc, char *argv[])
{
	int i, s, id;
	int num_slaves;
	int characters;
	char *message;
	char command[128];

	if (argc < 2)
	{
		printf("[MASTER] Insert the desired number of slave processes: ");
		if (scanf("%d", &num_slaves) != 1 || num_slaves < 1)
			return -1;
		getchar();

		FILE *filp[num_slaves];

		for (i=0; i<num_slaves; i++)
		{
			characters = snprintf(command, 128, "%s %d", argv[0], i);

			if (characters > 0 && characters < 128)
				filp[i] = popen(command, "w");
			else
				filp[i] = NULL;
		}

		master_function(filp, num_slaves);

		for (i=0; i<num_slaves; i++)
			if (filp[i] != NULL)
				pclose(filp[i]);
	}
	else
	{
		id = atoi(argv[1]);

		for (s=0; s<STEPS; s++)
		{
			scanf("%m[^\n]", &message);
			getchar();

			printf("[SLAVE-%d] Received message: %s\n", id, message);

			free(message);
		}
	}

	return 0;
}