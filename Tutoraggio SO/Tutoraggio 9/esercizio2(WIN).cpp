#include <windows.h>
#include <string.h>
#include <stdio.h>


#define N_PAGES 1024


static DWORD page_size;
static DWORD page_bitmap[N_PAGES];

static void* pages;


static int init_memory_allocator(void)
{
	int i;
	SYSTEM_INFO info;

	/*
	 * La funzione "GetSystemInfo" ci ritorna un insieme di informazioni
	 * del sistema tra cui la dimensione della pagina che ci interessa.
	 */
	GetSystemInfo((LPSYSTEM_INFO)&info);
	page_size = info.dwPageSize;

	for (i = 0; i < N_PAGES; i++)
		page_bitmap[i] = (DWORD) 0;

	/*
	 * Invochiamo la funzione "VirtualAlloc" per riservarci un certo numero di
	 * pagine contigue inzializzate a zero.
	 *
	 * Impostando il primo parametro a NULL lasciamo al sistema la decisione
	 * dell'indirizzo iniziale dove andare a riservare le pagine richieste.
	 *
	 * Il secondo parametro rappresenta la dimensione di memoria da riservare
	 * che, dato che il primo parametro passato è NULL, questo valore è
	 * arrotondato per eccesso per coincidere con boundary della pagina.
	 *
	 * Il parametro MEM_COMMIT garantisce che la memoria riservata sia subito
	 * accedibile ed inizializzata a zero. Le pagine virtuali hanno il
	 * corrispettivo frame di memoria fisica materializzato.
	 *
	 * L'ultimo parametro determina il tipo di protezione da impostare per
	 * le pagine riservate.
	 */
	if ((pages = VirtualAlloc(NULL, (SIZE_T)(N_PAGES * page_size), MEM_COMMIT, PAGE_NOACCESS)) == NULL)
		return -1;

	return 0;
}


static int fini_memory_allocator(void)
{
	/*
	 * Invocando "VirtualFree" passandogli come primo parametro
	 * l'indirizzo di memoria riservata con "VirtualAlloc" permette
	 * di rilasciare l'intera memoria precedentemente richiesta con
	 * il comando MEM_RELEASE.
	 */
	return (VirtualFree(pages, 0, MEM_RELEASE)) ? 0 : -1;
}


int main(int argc, char* argv[])
{
	DWORD i, j, found, old_protect;
	DWORD answer, first, contiguous;

	if (init_memory_allocator() == -1)
	{
		printf("[EROOR] - Unable to initialize the Memory Allocator. Exit.\n");
		return -1;
	}

	while (1)
	{
		printf("\nEseguire una delle seguenti operazioni.\n");
		printf("\t[1] - Richiesta pagine contigue\n");
		printf("\t[2] - Liberare pagine contigue\n");
		printf("\t[3] - Genera il Dump della Bitmap\n");
		printf("\t[4] - Uscire\n");

		printf("\nInserire il numero corrispondente: ");

		if (scanf_s("%u", &answer) == 0)
		{
			printf("[WARNING] - Non valid input. Retry.\n");
			continue;
		}

		if (answer == 1)
		{
			printf("\nInserire il numero di pagine contigue desiderate: ");

			if (scanf_s("%u", &contiguous) == 0 || contiguous == 0 || contiguous > N_PAGES)
			{
				printf("[WARNING] - Non valid input. Retry.\n");
				continue;
			}

			found = 0;

			for (i = 0; i < (N_PAGES - contiguous); i++)
			{
				if (page_bitmap[i] == (DWORD) 0)
				{
					found = 1;

					for (j = 0; j < contiguous; j++)
					{
						if (page_bitmap[i + j] == (DWORD) 1)
						{
							found = 0;
							i = i + j + 1;
							break;
						}
					}

					if (found) break;
				}
			}

			if (found)
			{
				/*
				 * La funzione "VirtualProtect" con il comando PAGE_READWRITE ha l'effetto di
				 * cambiare la protezione di un sottoinsieme di pagine precedentemente riservate
				 * con "VirtualAlloc". In questo caso per permettere la lettura e la scrittura su
				 * corrispettivi frame in memoria fisica.
				 */
				if (VirtualProtect((LPVOID)(((unsigned long long int) pages) + ((unsigned long long int) i * page_size)), (SIZE_T)(contiguous * page_size), PAGE_READWRITE, (PDWORD)&old_protect) == 0)
				{
					printf("[WARNING] - Unable to update page access mode.\n");
					continue;
				}

				for (j = i; j < (i + contiguous); j++)
					page_bitmap[j] = (DWORD) 1;

				printf("[INFO] - %u contiguous pages have been allocated.\n", contiguous);
			}
			else
			{
				printf("[INFO] - There are no %u contiguous free pages.\n", contiguous);
				continue;
			}
		}
		else if (answer == 2)
		{
			printf("\nInserire l'indice della prima pagina da liberare: ");

			if (scanf_s("%u", &first) == 0 || first >= N_PAGES)
			{
				printf("[WARNING] - Non valid input. Retry.\n");
				continue;
			}

			if (page_bitmap[first] == (DWORD) 0)
			{
				printf("[INFO] - Page at index %u was not allocated.\n", first);
				continue;
			}

			printf("Inserire il numero massimo di pagine contigue da liberare: ");

			if (scanf_s("%u", &contiguous) == 0 || contiguous == 0 || (first + contiguous) > N_PAGES)
			{
				printf("[WARNING] - Non valid input. Retry.\n");
				continue;
			}

			found = 1;

			for (i = first; i < (first + contiguous); i++)
			{
				if (page_bitmap[i] == (DWORD) 0)
				{
					found = 0;
					break;
				}
			}

			if (found)
			{
				/*
				 * La funzione "VirtualProtect" con il comando PAGE_NOACCESS ha l'effetto di
				 * cambiare la protezione di un sottoinsieme di pagine precedentemente riservate
				 * con "VirtualAlloc". In questo caso per impedire qualsiasi tipo di operazione
				 * sui dati contenuti nei corrispettivi frame di memoria fisica.
				 */
				if (VirtualProtect((LPVOID)(((unsigned long long int) pages) + ((unsigned long long int) first * page_size)), (SIZE_T)(contiguous * page_size), PAGE_NOACCESS, (PDWORD)&old_protect) == 0)
				{
					printf("[WARNING] - Unable to update page access mode.\n");
					continue;
				}

				for (i = first; i < (first + contiguous); i++)
					page_bitmap[i] = (DWORD) 0;

				printf("[INFO] - %u contiguous pages have been freed.\n", contiguous);
			}
			else
			{
				printf("[INFO] - There are less than %u allocated pages starting from index %u.\n", contiguous, first);
				continue;
			}
		}
		else if (answer == 3)
		{
			printf("\n");
			for (i = 0; i < N_PAGES; i++)
				printf("%d", page_bitmap[i]);
			printf("\n");
		}
		else if (answer == 4)
		{
			break;
		}
		else
		{
			printf("[WARNING] - The operation requested does not exists. Retry.\n");
			continue;
		}
	}

	if (fini_memory_allocator() == -1)
	{
		printf("[EROOR] - Unable to finalize the Memory Allocator. Exit.\n");
		return -1;
	}
}
