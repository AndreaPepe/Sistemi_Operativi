#include <windows.h>
#include <stdio.h>

/***************************************************
 *                                                 *
 * Con questo semplice esempio di codice facciamo  *
 * un'indagine byte-a-byte del contenuto di una    *
 * stringa di "wchar_t".                           *
 *                                                 *
 ***************************************************/

#define N 128

int main(int argc, char* argv[], char* envp[])
{
    unsigned i;
    char res[1];

    char char_buf[] = "Una sequenza di caratteri!";
    wchar_t wchar_buf[] = L"Una sequenza di caratteri!";

    size_t char_buf_len = strlen(char_buf);
    size_t wchar_buf_len1 = strlen((LPCSTR) wchar_buf);
    size_t wchar_buf_len2 = wcslen(wchar_buf);

    printf("Length of \"char_buf_len\" = %lu\n", char_buf_len);
    printf("Length of \"(char *) wchar_buf_len\" = %lu\n", wchar_buf_len1);
    printf("Length of \"wchar_buf_len\" = %lu\n", wchar_buf_len2);

    scanf_s("%1c", res, (unsigned) _countof(res));

    printf("Content of \"char_buf_len\" = %s\n", char_buf);
    printf("Content of \"(char *) wchar_buf_len\" = %s\n", (LPCSTR) wchar_buf);
    wprintf(L"Content of \"wchar_buf_len\" = %s\n", wchar_buf);

    scanf_s("%1c", res, (unsigned)_countof(res));

    printf("Content (char-by-char) of \"(char *) wchar_buf_len\" = ");
    for (i=0; i<(2*wchar_buf_len2); i++)
        printf("%c", ((LPCSTR)wchar_buf)[i]);
    printf("\n");

    ExitProcess(0);
}