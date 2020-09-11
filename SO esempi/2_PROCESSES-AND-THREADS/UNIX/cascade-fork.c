#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define NUM_FORKS 10

int main(int a, char ** b){

	int residual_forks = NUM_FORKS;

	another_fork:		// label used with goto

	residual_forks--;
	if(fork()>0){		// processes who create child going pause
		pause();
	}
	else{
		sleep(3);
		if(residual_forks>0){	;  // child creates another child
		 	goto another_fork;	
		}
	}
	pause();



}
