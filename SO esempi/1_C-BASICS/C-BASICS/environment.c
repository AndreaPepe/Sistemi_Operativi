
//please compile with -nostartfiles

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

extern char** environ;	// extern indica una variabile di sistema esterna

void _start(void){
   	 char ** addr = environ;  // array di pointers a stringhe

         printf("environ head pointer is at address: %lu\n",(unsigned long)environ);
	 
	 while(*addr){
      	      printf("%s\n",*(addr));	// stampa le stringhe (var di sistema)
 	      addr++;
	}
	exit(0);

}
