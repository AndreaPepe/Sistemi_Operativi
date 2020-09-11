#include <stdio.h>
#include <windows.h>

#define Error_(x) { \
	puts(x); \
	ExitProcess(1); \
}

int main(int argc, char* argv[])
{
	char messaggio[30];
	SECURITY_ATTRIBUTES security;
	BOOL rit;
	HANDLE readHandle, writeHandle, temp_readHandle;
	DWORD result;
	BOOL newprocess;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	if (argc == 1)
	{
		size_t slength;
		char text[1024];

		security.nLength = sizeof(security);
		security.lpSecurityDescriptor = NULL;
		security.bInheritHandle = TRUE;

		/*
		 * Crea una pipe e ritorna gli handles per i canali di lettura
		 * e scrittura su di esso.
		 *
		 * La struttura SECURITY_ATTRIBUTES ha il campo bInheritHandle
		 * settato a TRUE per indicare che gli handle devono essere
		 * ereditati dai processi figli.
		 */
		rit = CreatePipe(&readHandle, &writeHandle, &security, 0);
		if (!rit)
			Error_("Errore nella CreatePipe");
		
		memset(&si, 0, sizeof(si));
		memset(&pi, 0, sizeof(pi));
		si.cb = sizeof(si);

		/*
		 * Per fare in modo che il processo generato possa utilizzare
		 * il canale di lettura della pipe ereditato, dobbiamo temporaneamente
		 * sostituire lo stdin con quest'ultimo.
		 */
		temp_readHandle = GetStdHandle(STD_INPUT_HANDLE);
		SetStdHandle(STD_INPUT_HANDLE, readHandle);

		slength = strlen(argv[0]);
		strcpy_s(text, (unsigned)1015, argv[0]);
		strcpy_s(&text[slength], (unsigned)9, " lettore\0");

		newprocess = CreateProcessA(
			(LPCSTR) argv[0],
			(LPSTR) text,
			NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL,
			(LPSTARTUPINFOA) &si, &pi
		);
		if (newprocess == 0)
			Error_("Errore nella generazione dello scrittore!\n");

		/*
		 * Reimpostiamo il canale di stdin originale per il processo
		 * padre e chiudiamo il canale di lettura.
		 */
		SetStdHandle(STD_INPUT_HANDLE, temp_readHandle);
		CloseHandle(readHandle);

		printf("digitare testo da trasferire (quit per terminare):\n");
		fflush(stdout);

		do
		{
			fgets(messaggio, 30, stdin);

			/*
			 * Scriviamo sul canale di scrittura della pipe un
			 * messaggio di al più 30 bytes.
			 */
			rit = WriteFile(writeHandle, messaggio, 30, &result, NULL);
			if (!rit)
				Error_("Errore nella writefile!");

			printf("scritto messaggio: %s", messaggio);
			fflush(stdout);
		}
		while (strcmp(messaggio, "quit\n") != 0);

		CloseHandle(writeHandle);

		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else if (argc == 2 && strcmp(argv[1], "lettore") == 0)
	{
		do
		{
			/*
			 * Il processo figlio legge dal canale di lettura della pipe
			 * un messaggio di al più 30 bytes.
			 */
			rit = ReadFile(GetStdHandle(STD_INPUT_HANDLE), messaggio, 30, &result, NULL);
			if (rit)
			{
				printf("letto messaggio: %s", messaggio);
				fflush(stdout);
			}

			messaggio[(result < 30) ? result : 29] = '\0';

			if (strcmp(messaggio, "quit\n") == 0)
				break;
		}
		while (rit);
	}
}
