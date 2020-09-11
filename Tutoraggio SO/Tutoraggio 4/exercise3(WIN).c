#include <windows.h>
#include <stdio.h>


constexpr auto NTHR = 4;


char msg[NTHR][128];


DWORD WINAPI thread_function(LPVOID lpParam)
{
	INT c;
	UINT id = (UINT)lpParam;

	if (id > 0)
	{
		printf("[Thread %u] Messaggio ricevuto: %s\n", id, msg[id - 1]);
	}

	if (id < NTHR - 1)
	{
		printf("[Thread %u] Inserisci un messaggio: ", id);
		scanf_s("%[^\n]", msg[id], 128);
		while ((c = getchar()) != (INT)'\n');
	}

	ExitThread(0);
}

int main()
{
	UINT i;
	DWORD hId[NTHR];
	HANDLE hThread[NTHR];

	for (i = 0; i < NTHR; i++)
	{
		if ((hThread[i] = CreateThread(
			NULL,										// LPSECURITY_ATTRIBUTES: determina se l'handle può essere
														//                        ereditato da processi figli.
			0,											// SIZE_T: taglia iniziale dello stack per il thread.
			(LPTHREAD_START_ROUTINE)thread_function,	// LPTHREAD_START_ROUTINE: funzione eseguita dal thread.
			(LPVOID) i,										// __drv_aliasesMem LPVOID: argomento per la funzione del
														//                          thread.
			(i > 0) ? 4 : 0,							// DWORD: se impostato a 0 il thread inizia ad eseguire
														//        immediatamente, se impostato a 4 (CREATE_SUSPENDED)
														//        il thread è creato in uno stato di sospensione e
														//        non riparte finché la funzione "ResumeThread" non
														//        viene invocata.
			&hId[i]										// LPDWORD: un puntatore ad una variabile intera che viene
														//          inizializzata con l'identificatore del thread.
		)) == NULL)
		{
			printf("Unable to create a new thread. Exit with code %lu.\n", GetLastError());
			ExitProcess(1);
		}
	}

	for (i = 0; i < NTHR; i++)
	{
		WaitForSingleObject(hThread[i], INFINITE);
		if (i < NTHR - 1)
			if (ResumeThread(hThread[i + 1]) == -1)
				ExitProcess(1);
	}

	ExitProcess(0);
}
