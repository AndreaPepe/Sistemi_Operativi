#include <windows.h>
#include <stdio.h>
#include <stddef.h>

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		size_t arg_len;
		char text[128];

		BOOL newprocess;
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		DWORD exitcode;

		LPVOID shMem;
		HANDLE hShMem;

		/* Since hFile is INVALID_HANDLE_VALUE, CreateFileMapping creates a file mapping object of a specified
		   size that is backed by the system paging file instead of by a file in the file system.  */
		if ((hShMem = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, "shared_memory")) == NULL)
			ExitProcess(-1);

		if ((shMem = MapViewOfFile(hShMem, FILE_MAP_ALL_ACCESS, 0, 0, 4096)) == NULL)
		{
			CloseHandle(hShMem);
			ExitProcess(-1);
		}

		arg_len = strlen(argv[0]);
		memcpy(text, argv[0], arg_len);

		memset(&si, 0, sizeof(si));
		memset(&pi, 0, sizeof(pi));
		si.cb = sizeof(si);

		memcpy(&text[arg_len], " producer", 10);

		if ((newprocess = CreateProcessA((LPCSTR)argv[0], (LPSTR)text, NULL, NULL, FALSE,
			NORMAL_PRIORITY_CLASS, NULL, NULL, (LPSTARTUPINFOA)&si, &pi)) == FALSE)
		{
			UnmapViewOfFile(shMem);
			CloseHandle(hShMem);
			ExitProcess(-1);
		}

		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &exitcode);
		GetExitCodeProcess(pi.hThread, &exitcode);

		if (exitcode == 0)
			printf("\nIl messaggio inserito è:  %s\n\n", (char*)shMem);

		UnmapViewOfFile(shMem);
		CloseHandle(hShMem);

		ExitProcess(exitcode);
	}
	else if (strcmp(argv[argc - 1], "producer") == 0)
	{
		LPVOID shMem;
		HANDLE hShMem;

		if ((hShMem = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "shared_memory")) == NULL)
			ExitProcess(-1);

		if ((shMem = MapViewOfFile(hShMem, FILE_MAP_ALL_ACCESS, 0, 0, 4096)) == NULL)
		{
			CloseHandle(hShMem);
			ExitProcess(-1);
		}

		printf("\nInserisci un messaggio:  ");
		scanf_s("%[^\n]", (char*)shMem, 4096);

		UnmapViewOfFile(shMem);
		CloseHandle(hShMem);

		ExitProcess(0);
	}
}
