#include <stdio.h>
#include <windows.h>

typedef struct {
	long code;
	char request_string[20];
} request;

int main(int argc, char* argv[])
{
	request r;
	char* response = (char*)"done";
	DWORD result;
	HANDLE my_pipe;

	while (1)
	{
		/*
		 * Crea un istanza di named pipe e ritorna un handle
		 * per le successive operazioni sulla pipe.
		 */
		my_pipe = CreateNamedPipeA(
			(LPCSTR)"\\\\.\\pipe\\serv",	// Il nome univoco della pipe.
			PIPE_ACCESS_DUPLEX,				// Il modo di apertura: PIPE_ACCESS_DUPLEX è bi-direzionale.
			PIPE_TYPE_BYTE,					// Il modo di utilizzo della pipe: PIPE_TYPE_BYTE indica stream di bytes.
			PIPE_UNLIMITED_INSTANCES,		// Il numero massimo di istanze che possono essere create per questa pipe.
			1024,							// Il numero di bytes riservati per l'output buffer.
			1024,							// Il numero di bytes riservati per l'input buffer.
			0,								// Valore di timeout per l'utilizzo della pipe: 0 è 50 millisecondi.
			NULL							// Descrittore di sicurezza di deafult.
		);

		if (my_pipe == INVALID_HANDLE_VALUE)
		{
			printf("Errore nella chiamata CreateNamedPipe\n");
			fflush(stdout);
			ExitProcess(1);
		}

		/*
		 * Abilita un processo server ad attendere la connessione sulla
		 * named pipe da parte di un processo client.
		 */
		if (!ConnectNamedPipe(my_pipe, NULL))
		{
			printf("Errore nella chiamata ConnectNamedPipe\n");
			fflush(stdout);
			ExitProcess(1);
		}

		/*
		 * Leggiamo dalla named pipe un numero di bytes pari
		 * alla dimensione della struttura request.
		 */
		if (!ReadFile(my_pipe, &r, sizeof(r), &result, NULL))
		{
			printf("Errore nella chiamata readfile\n");
			ExitProcess(1);
		}

		printf("service request (code is %d - payload string is = %s)\n", r.code, r.request_string);
		fflush(stdout);

		/*
		 * Scriviamo sulla named pipe il messagio di risposta "done".
		 */
		if (!WriteFile(my_pipe, response, 20, &result, NULL))
		{
			printf("Errore nella chiamata WriteFile\n");
			ExitProcess(1);
		}
	}

	return(0);
}