#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>


int main (int argc, char **argv){

	int fd;
	size_t len;
	pid_t pid;

	if((fd=open("file_prova",O_CREAT|O_TRUNC|O_RDWR, 0666)) == -1){
		printf("Error opening file occurred!\n");
		exit (-1);
	}	
	
	if((pid=fork()) == -1){
		printf("Fork() error!\n");
		exit(-1);
	}

	if(pid==0){
		char *buff;
		printf("[Child] Please insert a message: ");
		//fflush(stdout);
		
		scanf("%m[^\n]", &buff);
		len=strlen(buff);
		
		write(fd,buff,len);
		free(buff);
		close(fd);
		exit(len);
	}
	else{
		int status;
		wait(&status);
		len=(size_t) status;
		char buff[len + 1];
		lseek(fd,0,SEEK_SET);

		read(fd,buff,len);

		buff[len]='\0';
		close(fd);		
		printf("[Main thread]: %s\n",buff);
		exit(0);
	}
	
}
