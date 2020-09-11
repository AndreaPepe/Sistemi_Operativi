#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define NUM_THREADS 1

#define MAX_STRING_LEN     4096 

#define NUM_TARGET_STRINGS 11

char* the_strings[NUM_TARGET_STRINGS] ; 

__thread char buffer[MAX_STRING_LEN];	// variabile TLS
__thread int occurrences = 0;

void * SearchThread(){

  int status;
  int i;

  while(scanf("%s",buffer) == 1){
	for(i=0;i<NUM_TARGET_STRINGS;i++){
		if(strstr(buffer,the_strings[i]) != NULL){	// strstr() verifica se buffer è contenuta in the_strings[i]
			occurrences++;
		}
	}
  }	
  printf("found %d occurrences of searched strings\n",occurrences);
  status = 0;
  pthread_exit((void *)&status);
} 

int main (int a, char **b )
{
  int i;
  pthread_t tid[NUM_THREADS];		// array di thread ID
 
  for(i=0; i<NUM_TARGET_STRINGS; i++){
	buffer[0] = '\0';
	scanf("%s",buffer);
	the_strings[i] = strdup(buffer);
        if (the_strings[i] == NULL) exit(-1);
  }

  for(i=0; i<NUM_THREADS; i++){
	pthread_create(&tid[i],NULL,SearchThread,NULL);
  }
  for(i=0; i<NUM_THREADS; i++){
	pthread_join(tid[i],NULL);
  }
  exit(0);
}

