#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _UNICODE

int wmain(int argc, wchar_t* argv[])
{
	BOOL newprocess;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD exitcode;

	wchar_t text[1024];

	size_t slength;

	wprintf(L"\nExecuting with UNICODE character set\n\n");

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);

	if (argc == 1) {
		slength = wcslen(argv[0]);
		wcscpy_s(text, (unsigned)1022, argv[0]);
		text[slength] = L' ';

		wprintf(L"Enter any string: ");
		wscanf_s(L"%[^\n]", &text[slength + 1], (unsigned)1022 - slength);

		newprocess = CreateProcessW((LPCWSTR)argv[0], (LPWSTR)text, NULL, NULL, FALSE,
			NORMAL_PRIORITY_CLASS, NULL, NULL, (LPSTARTUPINFOW)&si, &pi);

		if (newprocess == FALSE) {
			wprintf(L"Unable to create new process\n");
			ExitProcess(-1);
		}

		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &exitcode);
		wprintf(L"Process with PID=%d has finished\n", GetCurrentProcessId());
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		ExitProcess(0);
	}
	else
	{
		int i;
		wprintf(L"Process with PID=%d inhered string: ", GetCurrentProcessId());
		for (i = 1; i < argc; i++)
			wprintf(L"%s ", (LPWSTR)argv[i]);
		wprintf(L"\n");
		ExitProcess(0);
	}
}

#else

int main(int argc, char* argv[])
{
	BOOL newprocess;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD exitcode;

	char text[1024];

	size_t slength;

	printf("\nExecuting with ASCII character set\n\n");

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);

	if (argc == 1) {
		slength = strlen(argv[0]);
		strcpy_s(text, (unsigned)1022, argv[0]);
		text[slength] = ' ';

		printf("Enter any string: ");
		scanf_s("%[^\n]", &text[slength + 1], (unsigned)1022 - slength);

		newprocess = CreateProcessA((LPCSTR)argv[0], (LPSTR)text, NULL, NULL, FALSE,
			NORMAL_PRIORITY_CLASS, NULL, NULL, (LPSTARTUPINFOA)&si, &pi);

		if (newprocess == FALSE) {
			printf("Unable to create new process\n");
			ExitProcess(-1);
		}

		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &exitcode);
		printf("Process with PID=%d has finished\n", GetCurrentProcessId());
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		ExitProcess(0);
	}
	else
	{
		int i;
		printf("Process with PID=%d inhered string: ", GetCurrentProcessId());
		for (i = 1; i < argc; i++)
			printf("%s ", (LPSTR)argv[i]);
		printf("\n");
		ExitProcess(0);
	}
}

#endif
