#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define N 4
#define W 100
#define SIZE 16
char* filename = "pid-test.txt";

int main(int argc, char **argv){

	int fd,status,i,id,len;
	pid_t pid;
	char mex[SIZE];

	struct flock lck;
	
	if ((fd = open(filename, O_CREAT|O_TRUNC|O_RDWR, 0666))==-1){
		printf("An open() error occured!\n");
		exit(-1);	
	}

	for (i=0;i<N;i++){
		
		if((pid=fork())==-1){
			printf("A fork() error occured!\n");
			exit(-1);
		}

		if(pid==0){		// child processes run this
		
			lck.l_type = F_WRLCK;
                	lck.l_whence = SEEK_SET;
                	lck.l_start = 0;
                	lck.l_len = 0;
                	lck.l_pid = 0;
			
			if(fcntl(fd, F_SETLKW, &lck)==-1){
				printf("An fcntl() error occured!\n");
				exit(-1);
			}
			
			id=(int)getpid();
			len=sprintf(mex,"%d",id);
			mex[len]=' ';

			for(i=0;i<W;i++){
				if(write(fd,mex,(size_t)(len+1))==-1){
					printf("A write() error occurred!\n");
					exit(-1);
				}
			}
				
			if(write(fd,"\n\n",2)==-1){
                        printf("A write() error occurred!\n");
                        exit(-1);
                        }
				
			lck.l_type=F_UNLCK;
			if(fcntl(fd,F_SETLKW,&lck)==-1){
				printf("An fcntl() error occured!\n");
	                        exit(-1);
			}

			close(fd);
			exit(0);			
		}

	}
	
	for(i=0;i<N;i++){
		wait(&status);
	}

	close(fd);
	exit(0);
}
