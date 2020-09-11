#include <windows.h>
#include <stdio.h>

/*******************************************************
 *                                                     *
 * Con questo semplice esempio di codice compilato     *
 * con il set di caratteri UNICODE andiamo a stampare  *
 * una sequenza di caratteri "char" utilizzando prima  *
 * un puntatore a "char", poi un puntatore a "wchar_t" *
 * ed infine stampiamo i caratteri "char" cast-ati a   *
 * "wchar_t".                                          *
 *                                                     *
 *******************************************************/

int main(int argc, char* argv[])
{
    int i;
    WCHAR wstr_buf[256];
    char *cstr = argv[0];
    WCHAR* wstr = (WCHAR*) argv[0];

    /*
     * "printf" per stampare una sequenza di caratteri
     * "char" puntati da variabile "char *".
     */
    i = 0;
    printf("Stringa 1:");
    while (cstr[i] != '\0')
        printf(" %c", cstr[i++]);
    printf("\n");

    /*
     * "wprintf" per stampare una sequenza di caratteri
     * "char" puntati da variabile "wchar_t *".
     */
    i = 0;
    wprintf(L"Stringa 2:");
    while (wstr[i] != L'\0')
        wprintf(L" %c", wstr[i++]);
    wprintf(L"\n");

    /*
     * Cast di caratteri "char" a "wchar_t".
     */
    i = 0;
    while (cstr[i] != '\0')
        wstr_buf[i] = (WCHAR)cstr[i++];
    wstr_buf[i] = L'\0';

    /*
     * "wprintf" per stampare una sequenza di caratteri
     * "wchar_t" puntati da variabile "wchar_t *".
     */
    i = 0;
    wprintf(L"Stringa 3:");
    while (wstr_buf[i] != L'\0')
        wprintf(L" %c", wstr_buf[i++]);
    wprintf(L"\n");

    ExitProcess(0);
}