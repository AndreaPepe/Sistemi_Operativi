#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
//#include <sys/mman.h>

char v[1024];


void generic_handler(int signal, siginfo_t *info, void* unused){

  sigset_t set;
			/* also printing where the fault is happened*/
	printf("received signal is %d - address is %p\n",signal,info->si_addr);
//	mmap(info->si_addr,4096,PROT_READ|PROT_WRITE,0,0,0);
	sleep(1);
}


int main(int argc, char **argv){

  int  i;
  char c;
  sigset_t set;
  struct sigaction act;
  char* addr = (char*)0xffffff00;


  sigfillset(&set);
  sigdelset(&set,SIGSEGV);
  sigdelset(&set,SIGINT);

  act.sa_sigaction = generic_handler; 
  act.sa_mask =  set;
  act.sa_flags = SA_SIGINFO;
  act.sa_restorer = NULL;
  sigaction(SIGSEGV,&act,NULL);


  while(1) {

	sleep(5);
	c = *addr;		// causes SIGSEGV
  }
  /* dopo il gestore il controllo viene ripreso dalla stessa operazione sopra e quindi va 
   * continuamente in SEGMENTATION FAULT (SIGSEGV) */
}
