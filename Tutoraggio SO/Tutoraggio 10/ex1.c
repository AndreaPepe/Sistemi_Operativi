#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define N 3

void* thread_function(void* arg){

	char* input_str, *string, *total_str;
	int tot_len, string_len, input_len;

	string = (char*)arg;
	printf("[Thread] Insert a string: ");
	scanf("%m[^\n]",&input_str);
	getchar();
	if (input_str != NULL)
		input_len=strlen(input_str);
	else
		input_len = 0;

	if (string != NULL)
		string_len=strlen(string);
	else
		string_len=0;


	tot_len=input_len + string_len + 2;
	total_str=malloc(tot_len*sizeof(char));
	
	if(input_len){
		if(string_len){
			strncpy(total_str,string,string_len);
			total_str[string_len]=' ';
			strncpy(total_str + string_len + 1, input_str, input_len);
			total_str[string_len + input_len + 1] = '\0';
		}
		else{
			strcpy(total_str,input_str);
		}

	}
	else if (string_len)
		strcpy(total_str, string);
	else
		total_str=NULL;
	

/*	if(string!=NULL)
		free(string);*/	
	string=total_str;
	free(input_str);

	pthread_exit((void*)string);
}




int main (int argc, char **argv){

	pthread_t tid;
	int i;
	char *string=NULL;	// prima stringa è NULL


	for(i=0;i<N;i++){
		if(pthread_create(&tid,NULL, thread_function, (void*)string)==-1){
			printf("Error pthread_create\n");
			exit(-1);
		}
		pthread_join(tid, (void**)&string);
	}

	printf("[Main thread] Total messages:  %s\n", string);
	return 0;
}
