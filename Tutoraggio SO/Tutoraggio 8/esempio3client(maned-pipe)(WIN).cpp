#include <stdio.h>
#include <windows.h>

#define SIZE 128

#define REQUEST_CODE 11

typedef struct {
	long code;
	char request_string[SIZE];
} request;

int main(int argc, char* argv[])
{
	DWORD ret;
	request r;
	char response[SIZE];
	HANDLE my_pipe;

	printf("Give me the payload string: ");
	fflush(stdout);

	scanf_s("%s", r.request_string, (unsigned)_countof(r.request_string));
	r.code = REQUEST_CODE;

	/*
	 * Apriamo la named pipe esistente in lettura e scrittura
	 */
	my_pipe = CreateFileA(
		"\\\\.\\pipe\\serv",
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (my_pipe == INVALID_HANDLE_VALUE)
	{
		printf("\n Service Unavailable \n");
		ExitProcess(1);
	}

	/*
	 * Scriviamo sulla named pipe tanti bytes quanto è la
	 * dimensione della struttura request che abbiamo
	 * inizializzato con una stringa presa dall'utente.
	 */
	if (!WriteFile(my_pipe, &r, sizeof(request), &ret, NULL))
	{
		printf("Errore nella chiamata WriteFile\n");
		ExitProcess(1);
	}

	/*
	 * Leggiamo dalla named pipe il messaggio di risposta
	 * da parte del processo server.
	 */
	if (!ReadFile(my_pipe, response, 20, &ret, NULL))
	{
		printf("Errore nella chiamata ReadFile\n");
		ExitProcess(1);
	}

	printf("server reply: %s\n", response);

	CloseHandle(my_pipe);

	return(0);
}