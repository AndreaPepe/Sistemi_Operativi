#include <stdio.h>

#ifdef MMCP
#include <string.h>
#endif

#include <stdlib.h>

int main()
{
	int buff_size = 0;
	char *dyn_buff;

	/*
	 * Utilizzaimo la stringa di formattazione "m"
	 * per far allocare alla funzione di libreria una
	 * porzione di memoria sufficiente a contenere
	 * l'intera frase passata da tastiera.
	 */
	scanf("%m[^\n]", &dyn_buff);
	
	/*
	 * Contiamo quindi il numero di caratteri (bytes)
	 * di cui è composta la frase passata da tastiera
	 * semplicemente verificando in quale posizione
	 * incontriamo il terminatore di stringa.
	 */
	while (dyn_buff[buff_size] != '\0')
		buff_size++;

	printf("\nStringa inserita nel buffer allocato dinamicamente:  %s\n", dyn_buff);
	printf("\nTaglia del buffer allocato dinamicamente:  %d\n", buff_size);

	/*
	 * Ci riserviamo sullo stack spazio sufficiente
	 * ad accogliere la stessa frase mantenuta nella
	 * heap, incluso un carattere finale di terminazione.
	 */
	char stk_buff[buff_size+1];

#ifdef MMCP
	/*
	 * La funzione "memcpy" copia buff_size bytes dalla
	 * area di memoria puntata da dyn_buff nell'aria di
	 * memoria puntata da stk_buff e ritorna il puntatore
	 * a stk_buff.
	 */
	memcpy(stk_buff, dyn_buff, buff_size);
#else
	int i;
	for (i=0; i<buff_size; i++)
		stk_buff[i] = dyn_buff[i];
#endif
	stk_buff[buff_size] = '\0';

	free(dyn_buff);

	printf("\nStringa copiata nel buffer allocato sullo stack:  %s\n\n", stk_buff);
}