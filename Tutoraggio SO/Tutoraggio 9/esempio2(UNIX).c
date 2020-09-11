#include <stdlib.h>
#include <stdio.h>

typedef struct __test__ {
	char string[100];
} test;

int main(void)
{
	int i;

	test *array[10];

	printf("\nAllochiamo 10 strutture dati aventi spazio per contenere 100 caratteri ciascuna.\n");

	for (i=0; i<10; i++)
		array[i] = (test *) malloc(sizeof(test));

	printf("\nInserire 10 stringhe differenti.\n\n");

	for (i=0; i<10; i++)
	{
		printf("[%d]: ", i);
		scanf("%s", array[i]->string);
	}

	printf("\nQuindi stampiamo le stringhe appena inserite.\n\n");

	for (i=0; i<10; i++)
		printf("[%d]: %s\n", i, array[i]->string);

	printf("\nLiberiamo le strutture ad indice 2 e 6.\n");

	free(array[2]);
	free(array[6]);

	printf("\nRi-stampiamo le stringhe appena inserite.\n\n");

	for (i=0; i<10; i++)
		printf("[%d]: %s\n", i, array[i]->string);

	printf("\n");
}