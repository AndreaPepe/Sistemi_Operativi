#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 1024
#define Error(x) { puts(x); exit(-1); }

char *r_file = "file_read.txt";
char *w_file = "write_file.txt";

int main(int argc, char **argv){

	int rf,wf;
	char buff[SIZE];
	char *token;

	if((rf=open(r_file,O_RDWR,0666))==-1)
		Error("Error occurred in the open syscall\n");

	if((wf=open(w_file,O_CREAT|O_TRUNC|O_WRONLY,0666))==-1)
		Error("Error occurred in the open syscall\n");

	
	while(fgets(buff,SIZE,(FILE*)r_file)!=NULL){
		
		token=strtok(buff," ");
		while(token!=NULL){
			
			if(strlen(token)>6){
				if(write(wf,token,strlen(token))==-1)
					Error("Error occurred in the write syscall\n");
			}

			token=strtok(NULL," ");
		}
	
	}

	close(rf); close(wf);
	exit(0);
}
