#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static const char *rfilename = "file-to-reverse.txt";
static const char *wfilename = "file-word-length.txt";


int main()
{
	FILE *rfile;
	FILE *wfile;

	char *line;
	char *token;

	if ((rfile = fopen(rfilename, "r")) == NULL) {
		printf("Unable to open file %s\n", rfilename);
		exit(-1);
	}

	if ((wfile = fopen(wfilename, "w")) == NULL) {
		printf("Unable to open file %s\n", wfilename);
		exit(-1);
	}

	/*
	 * Leggiamo dal file riga per riga chiedendo
	 * alla funzione di libreria "fscanf" di allocare
	 * per noi un buffer sufficientemente ampio per
	 * ospitare la linea di testo appena letta.
	 */
	while (fscanf(rfile, "%m[^\n]", &line) != EOF)
	{
		fgetc(rfile);

		/*
		 * Estraiamo dalla linea appena letta tutte le parole
		 * che che sono delimitate da uno spazio. Per ottenere
		 * questo risultato utilizziamo la funzione di libreria
		 * "strtok".
		 */
		token = strtok(line, " ");

		while (token != NULL)
		{
			/*
			 * Se la lunghezza della parola appena letta è
			 * superiore ai 6 caratteri, allora la possiamo
			 * scrivere sul file di destinazione.
			 */
			if (strlen(token) > 6)
			{
				if (fprintf(wfile, "%s ", token) < 0) {
					printf("Unable to write on file %s\n", wfilename);
					exit(-1);
				}
			}

			token = strtok(NULL, " ");
		}

		if (fprintf(wfile, "\n") < 0) {
			printf("Unable to write on file %s\n", wfilename);
			exit(-1);
		}

		/*
		 * Utilizziamo la funzione di libreria "fflush" per
		 * forzare il flush di tutti i dati bufferizzati a
		 * lato user-space sul buffer-cache in kernel-space.
		 */
		fflush(wfile);

		free(line);
	}

	/*
	 * In maniera analoga a quanto visto con "fflush", la system call
	 * "fsync" richiede in modo bloccante il trasferimento del contenuto
	 * del buffer-cache a lato kernel-space sul device sottostante (e.g.,
	 * hard disk). Al ritorno abbiamo reale garanzia che i dati siano
	 * stati storati su memoria permanente.
	 *
	 * La funzione "fileno" restituisce il descrittore di file associato
	 * alla stream di I/O passato come argomento.
	 */
	fsync(fileno(wfile));

	fclose(wfile);
	fclose(rfile);

	exit(0);
}
