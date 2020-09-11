#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define SIZE 1024

char* filename = "file.txt";


int main (int argc,char** argv){
	
	int fd[2];
	int prod, cons;
	int file = open(filename, O_CREAT|O_TRUNC|O_RDWR, 0666);	

	pipe(fd);

	prod=fork();
	if(prod==0){
		close(fd[0]);
		char buff[SIZE];
		int ret, res=0;

		printf("[PRODUCER] Insert strings:\n");
		fflush(stdout);

		do{
			fgets(buff,SIZE,stdin);
			write(fd[1],buff,SIZE);
			
		}while(strcmp(buff,"quit\n")!=0);

		close(fd[1]);
		exit(0);	
	}	

	cons=fork();
	if(cons==0){
		close(fd[1]);
		char buff[SIZE];
		char* buf2=buff;
		int len;
		printf("[CONSUMER] I'm alive\n");


		while(read(fd[0],buff,SIZE)){
			len=strlen(buff);
		//	printf("%s\n",buff);	
			write(file,buff,len);
		}

		close(fd[0]);
		exit(0);
	}


	wait(NULL);
	exit(0);

}
