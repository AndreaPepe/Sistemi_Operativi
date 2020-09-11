#include <windows.h>
#include <stdio.h>

/*****************************************************
 *                                                   *
 * Questo semplice esempio di codice è utile per     *
 * osservare empiricamente la taglia del tipo di     *
 * dato TCHAR quando il set di caratteri del         *
 * progetto è impostato a UNICODE o meno.            *
 *                                                   *
 * Per fare le prove seguire i seguenti passi:       *
 *                                                   *
 * - entrare nelle "proprietà" del progetto          *
 * - navigare fino alla scheda "avanzate"            *
 * - impostare il "set di caratteri" a UNICODE       *
 * - compilare ed eseguire il progetto               *
 *                                                   *
 * - entrare nelle "proprietà" del progetto          *
 * - navigare fino alla scheda "avanzate"            *
 * - impostare il "set di caratteri" a Non impostato *
 * - compilare ed eseguire il progetto               *
 *                                                   *
 *****************************************************/

int main(int argc, char* argv[], char* envp[])
{
	size_t char_size = sizeof(char);
	size_t tchar_size = sizeof(TCHAR);
	size_t wchar_size = sizeof(WCHAR);
	
	printf("SizeOf(char) = %lu\n", char_size);
	printf("SizeOf(TCHAR) = %lu\n", tchar_size);
	printf("SizeOf(WCHAR) = %lu\n", wchar_size);

	ExitProcess(0);
}