#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE 1024

char *filename;

void parent_function(int file, int fd){
	char buffer[SIZE];
	int ret;

	printf("[Parent] Insert please: ");
retry:	
	ret=scanf("%[^\n]",buffer);
	getchar();
	if(ret==-1){
		if(errno==EINTR)
			goto retry;
		else{
			printf("scanf error\n");
			exit(-1);
		}
	}
retry2:
	ret=write(fd,buffer,strlen(buffer)+1);
	if(ret==-1){
                if(errno==EINTR)
                        goto retry2;
                else{
                        printf("write error\n");
                        exit(-1);
                }
        }
	close(fd);
	return;
}
	
void child_function(int file, int fd){
        char buffer[SIZE];
        int ret;

        ret=read(fd,buffer,SIZE);
        if(ret==-1){
               printf("scanf error\n");
               exit(-1);
                }
        printf("\n%s\n",buffer);
	int res_r, res_w, prev_w;
	res_r=ret;
	res_w=prev_w=0;
	do{
        	prev_w += res_w ;
		res_w=write(file,&buffer[prev_w],res_r-prev_w);
		if(res_w==-1){
			exit(-1);
		}
	}while(res_w+prev_w<res_r);
	close(fd);
	close(file);
	return;
}


int main(int argc, char **argv){
	int ret;
	int fd[2];
	int file;
	pid_t pid;
	
	if(argc<2){
		printf("Usage: %s filename\n",argv[0]);
		exit(-1);
	}
	
	filename=argv[1];
	file=open(filename, O_CREAT|O_TRUNC|O_RDWR, 0666);
	if(file==-1){
		printf("open error\n");
		exit(-1);
	}

	ret=pipe(fd);
	if(ret==-1){
		printf("pipe error\n");
		exit(-1);
	}
	
	pid=fork();
	if(pid==-1){
		printf("fork failure\n");
		exit(-1);
	}
	if(pid==0){
		close(fd[1]);	//chiudo scrittura
		child_function(file,fd[0]);
	}

	else{
		close(fd[0]);
	//	signal(SIGPIPE,handler);
		parent_function(file,fd[1]);
	}
	return 0;
}
