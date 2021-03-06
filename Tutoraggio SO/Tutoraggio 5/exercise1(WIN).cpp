#include <windows.h>
#include <stdio.h>


constexpr auto CPUs = 4;
DWORDLONG loop_cycles = (1000ULL * 1000ULL * 1000ULL);


DWORD WINAPI thread_function(LPVOID arg)
{
	DWORDLONG i = *((DWORDLONG*)arg);
	DWORDLONG tmp;

	SetThreadAffinityMask(GetCurrentThread(), 1 << i);

	for (i = 0; i < loop_cycles; i++) {
		/* dummy computation */
		if (i % CPUs == 0)
			tmp = i * i;
	}

	ExitThread(0);
}


int main()
{
	DWORD hId, exitCode;
	HANDLE hThread;
	int i;

	for (i = 0; i < CPUs; i++) {
		if (!(hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_function, &i, 0, &hId))) {
			printf("Error. Unable to create a new thread.\n");
			return 1;
		}
		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &exitCode);
		printf("Thread has terminated with exit code %d.\n", exitCode);
	}

	return 0;
}
