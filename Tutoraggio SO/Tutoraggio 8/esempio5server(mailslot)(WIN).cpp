#include <windows.h>
#include <stdio.h>
#include <malloc.h>

constexpr auto MSIZE = 128;
constexpr auto MAX_NAME_SIZE = 128;

typedef struct {
	char queue_name[MAX_NAME_SIZE];
	int service_code;
} request;

typedef struct {
	request req;
} request_msg;

typedef struct {
	char mtext[MSIZE];
} response_msg;

HANDLE my_slot;
char *response = (char *) "done";

void * service_thread(request_msg *request_message)
{
	response_msg response_message;
	HANDLE response_slot;
	DWORD writtenchars;

	printf("asked service of type %d - response channel is %s\n", request_message->req.service_code, request_message->req.queue_name);

	/*
	 * Il messaggio inviato dal client contiene una stringa che rappresenta
	 * il niome della mailslot sulla quale il client vuole comunicare in
	 * maniera esclusiva con il server per ricevere un messaggio di risposta.
	 *
	 * Apriamo quindi la mailslot indicata dal processo client in scrittura
	 * e condivisa per la lettura da parte del client.
	 */
	response_slot = CreateFileA(
		request_message->req.queue_name,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (response_slot == INVALID_HANDLE_VALUE)
	{
		printf("cannot open response queue to the client\n");
#ifdef MULTITHREAD
		ExitThread(-1);
#else
		return NULL;
#endif
	}

	memcpy(response_message.mtext, response, 10);

	/*
	 * Scriviamo sulla mailslot del client un messaggio di risposta. La funzione
	 * "WriteFile" ritornerà solo quando tutti i bytes indicati saranno scritti
	 * nella mailslot. La chiamata è sincrona, ed il buffer è disponibile per
	 * poter essere eventualmente sovrascritto con un nuovo messaggio in caso
	 * ve ne fosse bisogno.
	 */
	if (WriteFile(response_slot, &response_message, MSIZE, &writtenchars, NULL) == 0)
	{
		printf("cannot return response to the client\n");
#ifdef MULTITHREAD
		ExitThread(-1);
#else
		return NULL;
#endif
	}

	free(request_message);

#ifdef MULTITHREAD
	ExitThread(0);
#else
	return NULL;
#endif
}

int main(int argc, char* argv[])
{
	void* request_message;

#ifdef MULTITHREAD
	DWORD thread_id;
#endif
	DWORD readchar;

	/*
	 * Creiamo una mailslot per il server il cui nome
	 * è univoco e conosciuto system-wide.
	 */
	my_slot = CreateMailslotA(
		"\\\\.\\mailslot\\server",
		sizeof(request_msg),
		MAILSLOT_WAIT_FOREVER,
		NULL
	);

	if (my_slot == INVALID_HANDLE_VALUE)
	{
		printf("cannot install server queue, please check the problem\n");
		exit(1);
	}
	
	while (1)
	{
		request_message = malloc(sizeof(request_msg));

		/*
		 * Ci mettiamo in ascolto sulla mailslot del server per la ricezione di un messaggio
		 * di almeno "sizeof(request_msg)" bytes. La funzione "ReadFile" ritorna solo quando
		 * il numero di bytes richiesti è stato letto.
		 */
		if (ReadFile(my_slot, request_message, sizeof(request_msg), &readchar, NULL) == 0)
		{
			printf("message receive error, please check the problem\n");
			ExitProcess(1);
		}
		else
		{
			/*
			 * Se abbiamo ricevuto un messaggio di richiesta correttamente, il server
			 * potrà evadere la richiesta in maniera sincrona o delegare un server-thread
			 * a tale scopo dipendentemente da se il flag MULTITHREAD è definito o meno.
			 */
#ifdef MULTITHREAD
			if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)service_thread, request_message, 0, &thread_id) == INVALID_HANDLE_VALUE)
			{
				printf("Cannot create service thread for error %d\n", GetLastError());
				ExitProcess(-1);
			}
#else
			service_thread((request_msg*)request_message);
#endif
		}
	}
}