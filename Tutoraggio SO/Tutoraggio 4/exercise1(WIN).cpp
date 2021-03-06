#include <windows.h>
#include <stdio.h>


/*
 * > https://en.wikipedia.org/wiki/Volatile_(computer_programming)
 * La keyword "volatile" indica al compilatore che il valore di una
 * variabile può cambiare tra due o più accessi differenti alla stessa
 * variabile, anche se questa non risulta essere modificata da alcuna
 * istruzione su quel flusso di controllo. Previene dunque che un
 * compilatore, nel apportare ottimizzazioni al codice, riusi un valore
 * "stale" in accessi consecutivi a quella variabile.
 *
 * > https://docs.microsoft.com/it-it/cpp/cpp/alignment-cpp-declarations?view=vs-2019
 * La keyword "__declspec((align(#))" permette di specificare l'allineamento
 * preciso di una variabile in memoria per sfruttare al massimo una specifica
 * architettura. Le variabili intere sono allineate su limiti di 4 bytes per
 * default, e questa parola chiave poteva essere omessa. E' utile al fine di
 * garantire che la memoria dove il contenuto di una variabile alloggia non
 * ricada in due linee di cache differenti.
 */
volatile __declspec(align(4)) DWORD shared_variable = 1;


DWORD WINAPI thread_function(LPVOID lpParam)
{
	printf("2. Child Thread is executing.\n");
	Sleep(5000);
	printf("3. Child Thread is terminating.\n");
	shared_variable = 0;
	ExitThread(0);
}


int main()
{
	DWORD hId;
	// DWORD exitCode;
	HANDLE hThread;
	printf("1. Parent Thread is executing.\n");
	if (!(hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_function, NULL, 0, &hId))) {
		printf("Unable to create a new thread. Exit.\n");
		ExitProcess(1);
	}
	else {
		while (shared_variable);
		printf("4. Parent Thread is terminating.\n");
		// GetExitCodeThread(hThread, & exitCode);   /* POTREBBE NON RITORNARE IL VALORE ATTESO ... PERCHè ? */
	}
	ExitProcess(0);
}
