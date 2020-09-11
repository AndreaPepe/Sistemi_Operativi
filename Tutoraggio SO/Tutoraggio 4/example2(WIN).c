#include <windows.h>
#include <stdio.h>


constexpr auto NTHR = 4;


DWORD WINAPI thread_function(LPVOID lpParam)
{
	UINT id = (UINT)lpParam;

	Sleep((id + 1) * 1000);

	printf("Thread %u ha terminato.\n", id);

	ExitThread(0);
}

int main()
{
	UINT i;
	DWORD hId[NTHR];
	HANDLE hThread[NTHR];

	for (i = 0; i < NTHR; i++)
	{
		if ((hThread[i] = CreateThread(NULL, 0,	(LPTHREAD_START_ROUTINE)thread_function, (LPVOID) i, 0, &hId[i])) == NULL)
		{
			printf("Unable to create a new thread. Exit with code %lu.\n", GetLastError());
			ExitProcess(1);
		}
	}

	WaitForMultipleObjects(
		NTHR,		// DWORD: numero di handles da gestire.
		hThread,	// const HANDLE *: un array di handles.
		TRUE,		// BOOL: se TRUE attende la terminazione
					//       di tutti gli oggetti, se FALSE
					//       si sblocca alla terminazione di
					//       un qualsiasi oggetto.
		INFINITE	// DWORD: timeout di attesa in millisecondi.
	);

	ExitProcess(0);
}
