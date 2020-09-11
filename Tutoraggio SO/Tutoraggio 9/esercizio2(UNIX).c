#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


#define N_PAGES 1024


static int page_size;
static int page_bitmap[N_PAGES];

static void * pages;


static int init_memory_allocator(void)
{
	int i;

	/*
	 * The function "getpagesize" returns
	 * the Linux-specific size of a page.
	 */
	page_size = getpagesize();

	for (i=0; i<N_PAGES; i++)
		page_bitmap[i] = 0;

	/*
	 * The function "mmap" creates a new mapping in the virtual
	 * address space of the calling process.
	 *
	 * The first argument set to NULL indicates that must be the
	 * kernel to choose the (page-aligned) address at which to
	 * create the mapping.
	 *
	 * The second argument indicates the size of memory to be
	 * mapped.
	 *
	 * The third argument describes the desired memory protection
	 * which is initially set to avoid page accesses.
	 *
	 * MAP_PRIVATE indicates to create a private copy-on-write
	 * mapping, while MAP_ANONYMOUS indicates that the mapping
	 * is not backed by any file and its content is zero-ed.
	 *
	 * The fifth argument is -1 and the sixth argument is 0
	 * since the mapping is not backed by any file.
	 */
	if ((pages = mmap(NULL, N_PAGES * page_size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
		return -1;

	return 0;
}


static int fini_memory_allocator(void)
{
	/*
	 * The function "munmap" deletes the mapping for the
	 * specified address range, and causes further references
	 * to addresses within the range to generate invalid
	 * memory references.
	 *
	 * The first argument must be a multiple of the page size.
	 */
	return munmap(pages, N_PAGES * page_size);
}


int main(int argc, char *argv[])
{
	int i, j, found;
	unsigned answer, first, contiguous;

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

		if (scanf("%u", &answer) == 0)
		{
			printf("[WARNING] - Non valid input. Retry.\n");
			continue;
		}

		if (answer == 1)
		{
			printf("\nInserire il numero di pagine contigue desiderate: ");

			if (scanf("%u", &contiguous) == 0 || contiguous == 0 || contiguous > N_PAGES)
			{
				printf("[WARNING] - Non valid input. Retry.\n");
				continue;
			}

			found = 0;

			for (i=0; i<(N_PAGES - contiguous); i++)
			{
				if (page_bitmap[i] == 0)
				{
					found = 1;

					for (j=0; j<contiguous; j++)
					{
						if (page_bitmap[i+j] == 1)
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
				 * The "mprotect" function changes the access protections for the calling
				 * process's memory pages containing any part of the address range in the
				 * interval [pages + (i * page_size), pages + (i * page_size) + (contiguous
				 * * page_size) - 1].
				 *
				 * The first argument must be aligned to a page boundary.
				 *
				 * PROT_READ|PROT_WRITE makes the memory pages accessible for the read/write
				 * operations.
				 */
				if (mprotect(pages + (i * page_size), contiguous * page_size, PROT_READ|PROT_WRITE) == -1)
				{
					printf("[WARNING] - Unable to update page access mode.\n");
					continue;
				}

				for (j=i; j<(i + contiguous); j++)
					page_bitmap[j] = 1;

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

			if (scanf("%u", &first) == 0 || first >= N_PAGES)
			{
				printf("[WARNING] - Non valid input. Retry.\n");
				continue;
			}

			if (page_bitmap[first] == 0)
			{
				printf("[INFO] - Page at index %u was not allocated.\n", first);
				continue;
			}

			printf("Inserire il numero massimo di pagine contigue da liberare: ");

			if (scanf("%u", &contiguous) == 0 || contiguous == 0 || (first + contiguous) > N_PAGES)
			{
				printf("[WARNING] - Non valid input. Retry.\n");
				continue;
			}

			found = 1;

			for (i=first; i<(first + contiguous); i++)
			{
				if (page_bitmap[i] == 0)
				{
					found = 0;
					break;
				}
			}

			if (found)
			{
				/*
				 * The "mprotect" function changes the access protections for the calling
				 * process's memory pages containing any part of the address range in the
				 * interval [pages + (first * page_size), pages + (contiguous * page_size)
				 * + (contiguous * page_size) - 1].
				 *
				 * The first argument must be aligned to a page boundary.
				 *
				 * PROT_NONE makes the memory pages not accessible.
				 */
				if (mprotect(pages + (first * page_size), contiguous * page_size, PROT_NONE) == -1)
				{
					printf("[WARNING] - Unable to update page access mode.\n");
					continue;
				}

				for (i=first; i<(first + contiguous); i++)
					page_bitmap[i] = 0;

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
			for (i=0; i<N_PAGES; i++)
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