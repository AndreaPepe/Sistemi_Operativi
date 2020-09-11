#include <windows.h>
#include <stdio.h>


constexpr auto THRs = 3;


DWORD WINAPI thread_function(LPVOID arg)
{
	DWORDLONG i, j;
	DWORD tid = *((PDWORD) arg);

	/*
	 * Tutti i Threads impostano l'affinità sulla sola
	 * CPU-core 0. Le loro esecuzioni saranno interleaved
	 * in fair time-sharing ... fintantoché non vi siano
	 * threads privilegiati.
	 */
	SetThreadAffinityMask(GetCurrentThread(), 1);

	/*
	 * Impostiamo priorità differenti per i differenti
	 * threads in esecuzione sulla stessa CPU-core 0.
	 * La condivisione del tempo di esecuzione non sarà
	 * più equamente ripartita.
	 */
	SetThreadPriority(GetCurrentThread(), (THREAD_PRIORITY_NORMAL + (int) tid));

	for (i = 0, j = 0; i < (1ULL << 48); i++)
	{
		if ((i % (1ULL << 27)) == 0)
		{
			printf("Thread %d has counted up to %I64u\n", tid, ++j);
			if (tid && (j % 10) == 0)
			{
				/*
				 * Per i threads con ID superiore (più alta priorità)
				 * inseriamo un tempo di sleep sufficiente a permettere
				 * ai threads con più bassa priorità di poter fare progressi.
				 */
				printf("Thread %d sleeps for %d seconds\n", tid, (tid * 4));
				Sleep((tid * 4) * 1000);
			}
		}
	}

	ExitThread(0);
}


int main()
{
	DWORD i;
	DWORD hId[THRs];
	HANDLE hThread[THRs];
	DWORD tid[THRs];

	for (i = 0; i < THRs; i++) {
		tid[i] = i;
		if (!(hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_function, (LPVOID) &tid[i], 0, &hId[i]))) {
			printf("Error. Unable to create a new thread.\n");
			return 1;
		}
	}

	WaitForMultipleObjects(THRs, hThread, TRUE, INFINITE);

	return 0;
}
