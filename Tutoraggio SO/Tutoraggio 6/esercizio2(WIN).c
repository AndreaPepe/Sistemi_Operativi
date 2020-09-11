#include <windows.h>
#include <stdio.h>

const char* filename = "read-write.txt";

int main(int argc, char *argv[])
{
	size_t slength;
	char text[1024];

	DWORD bytes;

	HANDLE file;
	SECURITY_ATTRIBUTES sa;

	if (argc > 1)
	{
		ZeroMemory(&sa, sizeof(sa));
		sa.nLength = sizeof(sa);

		if ((file = CreateFileA(filename, GENERIC_WRITE, 0, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
			return GetLastError();

		printf("Inserisci un messaggio sul file: ");
		scanf_s("%[^\n]", text, (unsigned)1023);
		text[1023] = '\0';
		slength = strlen(text);

		if (WriteFile(file, text, slength, &bytes, NULL) == FALSE)
		{
			CloseHandle(file);
			return GetLastError();
		}

		CloseHandle(file);
	}
	else
	{
		BOOL newprocess;
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		slength = strlen(argv[0]);
		strcpy_s(text, (unsigned)1021, argv[0]);
		strcpy_s(&text[slength], (unsigned)4, " CP\0");

		if ((newprocess = CreateProcessA((LPCSTR)argv[0], (LPSTR)text, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, (LPSTARTUPINFOA)&si, &pi)) == FALSE)
			return GetLastError();

		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		ZeroMemory(&sa, sizeof(sa));
		sa.nLength = sizeof(sa);

		if ((file = CreateFileA(filename, GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
			return GetLastError();

		if (ReadFile(file, text, 1024, &bytes, NULL) == FALSE)
		{
			CloseHandle(file);
			return GetLastError();
		}

		text[bytes] = '\0';

		printf("Messaggio letto dal file: %s\n", text);

		CloseHandle(file);
	}

	return 0;
}
