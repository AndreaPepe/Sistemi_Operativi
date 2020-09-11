#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

constexpr auto MSIZE = 128;
constexpr auto MAX_NAME_SIZE = 128;

constexpr auto REQUEST_CODE = 1;

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

HANDLE my_slot_id, server_slot_id;
long key;

char queue_name[MAX_NAME_SIZE];
request_msg  request_message;
response_msg response_message;

int main(int argc, char* argv[])
{
	DWORD writtenchars, readchars;

	key = GetCurrentProcessId();

	/*
	 * Utilizziamo il PID del processo per creare un nome di mailslot
	 * univuco su cui il client vorrà comunicare in maniera esclusiva
	 * con il server.
	 */
	sprintf_s(queue_name, MAX_NAME_SIZE, "\\\\.\\mailslot\\client%d", key);

	/*
	 * Creiamo immediatamente la mailslot del client per fare in modo
	 * che sia subito disponibile quando passeremo il suo nome al server.
	 */
	my_slot_id = CreateMailslotA(
		queue_name,
		sizeof(response_msg),
		MAILSLOT_WAIT_FOREVER,
		NULL
	);
	
	if (my_slot_id == INVALID_HANDLE_VALUE)
	{
		printf("cannot install client mail-slot with name %s, please check with the problem\n", queue_name);
		fflush(stdout);
		ExitProcess(1);
	}

	/*
	 * Apriamo la mailslot del server su cui invieremo il messaggio
	 * contenente il nome della mailslot del client.
	 */
	server_slot_id = CreateFileA(
		"\\\\.\\mailslot\\server",
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (server_slot_id == INVALID_HANDLE_VALUE)
	{
		printf("cannot open server mail-slot, please check with the problem\n");
		ExitProcess(1);
	}

	request_message.req.service_code = REQUEST_CODE;

	strcpy_s(request_message.req.queue_name, MAX_NAME_SIZE, queue_name);

	/*
	 * Scriviamo sulla mailslot del server il messaggio contenente
	 * il nome della mailslot del client.
	 */
	if (WriteFile(server_slot_id, &request_message, sizeof(request), &writtenchars, NULL) == 0)
	{
		printf("cannot send request to the server\n");
		fflush(stdout);
		ExitProcess(1);
	}

	/*
	 * Leggiamo dalla mailslot del client il messaggio di risposta dal server.
	 */
	if (ReadFile(my_slot_id, &response_message, MSIZE, &readchars, NULL) == 0)
	{
		printf("error while receiving the server response, please check with the problem\n");
		fflush(stdout);
		ExitProcess(1);
	}
	else
	{
		printf("server response: %s\n", response_message.mtext);
		fflush(stdout);
		ExitProcess(0);
	}
}