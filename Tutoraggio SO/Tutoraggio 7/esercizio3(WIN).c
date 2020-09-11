#include <windows.h>
#include <stdio.h>

constexpr auto N = 8;
constexpr auto W = 20;

const char* filename = "read-write.txt";

int main(int argc, char* argv[])
{
	DWORD i;

	size_t slength;
	char text[1024];

	if (argc > 1)
	{
		DWORD bytes;

		HANDLE file;
		SECURITY_ATTRIBUTES sa;

		ZeroMemory(&sa, sizeof(sa));
		sa.nLength = sizeof(sa);

		sprintf_s(text, 1024, "%d ", GetCurrentProcessId());
		text[1023] = '\0';
		slength = strlen(text);

		if ((file = CreateFileA(filename, GENERIC_WRITE, 0, &sa, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
			if ((file = CreateFileA(filename, GENERIC_WRITE, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
				return GetLastError();

		printf("QUI!");

		LockFile(file, 0, 0, (1UL << 31), 0);

		SetFilePointer(file, 0, NULL, FILE_END);

		for (i = 0; i < W; i++)
		{
			if (WriteFile(file, text, slength, &bytes, NULL) == FALSE)
			{
				CloseHandle(file);
				return GetLastError();
			}
		}

		UnlockFile(file, 0, 0, (1UL << 31), 0);

		CloseHandle(file);
	}
	else
	{
		BOOL newprocess;
		STARTUPINFO si[N];
		PROCESS_INFORMATION pi[N];

		slength = strlen(argv[0]);
		strcpy_s(text, (unsigned)1021, argv[0]);
		strcpy_s(&text[slength], (unsigned)4, " CP\0");

		for (i = 0; i < N; i++)
		{
			ZeroMemory(&si[i], sizeof(si[i]));
			si[i].cb = sizeof(si);
			ZeroMemory(&pi[i], sizeof(pi[i]));

			if ((newprocess = CreateProcessA((LPCSTR)argv[0], (LPSTR)text, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, (LPSTARTUPINFOA)&si[i], &pi[i])) == FALSE)
				return GetLastError();
		}

		for (i = 0; i < N; i++)
		{
			WaitForSingleObject(pi[i].hProcess, INFINITE);
			CloseHandle(pi[i].hProcess);
			CloseHandle(pi[i].hThread);
		}
	}

	return 0;
}
