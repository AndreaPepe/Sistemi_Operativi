#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char c;
	int i, j;
	char buff[21];

	if (argc < 2)
	{
		printf("Nessuna stringa passata come argomento.\n");
		return 0;
	}

	/*
	 * Copiamo nel buffer sullo stack la prima stringa
	 * passata come argomento alla funzione main. Lo
	 * facciamo copiando tutti i caratteri fino al
	 * raggiungimento del primo terminatore di stringa
	 * od alla fine della taglia del buffer utilizzato.
	 */
	for (i=0; i<20; i++)
	{
		if (argv[1][i] == '\0')
			break;
		buff[i] = argv[1][i];
	}
	buff[i] = '\0';

	printf("%s\n", buff);

	/*
	 * Effettuiamo una spostamento efficiente dei caratteri
	 * nel buffer in-place, senza utilizzare un buffer
	 * ausiliario, ma invertendo in maniera speculare
	 * la prima metà del buffer con la seconda.
	 */
	for (j=0; j<(i/2); j++)
	{
		c = buff[i-j-1];
		buff[i-j-1] = buff[j];
		buff[j] = c;
	}

	printf("%s\n", buff);

	/*
	 * La funzione "memset" setta 21 bytes a partire
	 * dal puntatore buff con il valore 0.
	 */
	memset(buff, 0, 21);
}