#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void thread_1(char* str){

	scanf("%m[^\n]",&str);
}

void thread_2(char* str){
	printf("%s",str);
}

int main(int argc,char** argv){
	pthread_t tid1, tid2;
	printf("Inserire un messaggio: ");
	fflush(stdout);
	char* str;

	int t1,t2;

	t1=pthread_create(&tid1,NULL,(void*) thread_1,&str);
	if(t1==0) exit(0);
	int* status1;
	pthread_join(t1,(void**)&status1);	
	
	t2=pthread_create(&tid2,NULL,(void*) thread_2,&str);
	if(t2!=0) exit(0);
}
