
/*********************************************************/
/*************** server process **************************/
/*********************************************************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "header.h"


int id_coda, ret, STATUS;
long key = 50;
char *response = "done";

request_msg  request_message;
request_msg  *new_message;
pthread_t tid;




void * do_work(request_msg *request_message){

    response_msg response_message;
    int status;

    printf("asked service of type %d - response channel is %d\n", request_message->req.service_code,request_message->req.response_channel); 
    sleep(5);

    response_message.mtype = 1;
    memcpy(response_message.mtext, response, strlen(response)+1);	// scrive "done\0"
    if ( msgsnd(request_message->req.response_channel, &response_message, SIZE, FLAG) == -1 ) {
	printf("cannot return response to the client\n");
	exit(-1);
    }

    #ifdef MULTITHREAD
    free(request_message);
    status = 0;
    pthread_exit((void *)&status);
    #endif

}/* end do_work */


int main(int argc, char *argv[]) {


    id_coda = msgget(key, IPC_CREAT|IPC_EXCL|0666);
    if( id_coda == -1){
	id_coda = msgget(key, IPC_CREAT|0666);
	ret = msgctl(id_coda,IPC_RMID,0);
	id_coda = msgget(key, IPC_CREAT|IPC_EXCL|0666);
	if( id_coda == -1 ){
	    printf("cannot install server queue, please check with the problem\n");
	    exit(EXIT_FAILURE);
	}	
    }


    while(1) {

	if ( msgrcv(id_coda, &request_message, sizeof(request), 1, FLAG) == -1) { 
	    printf("message receive error, please check with the problem\n");
	    fflush(stdout);
	}
	else {

	    #ifdef MULTITHREAD 
	        new_message = malloc(sizeof(request_msg));
		if(!new_message){
		    printf("cannot allocate new request message buffer \n");
		    fflush(stdout);
		    exit(EXIT_FAILURE);
		}
		memcpy((char*)new_message,(char*)&request_message,sizeof(request_msg));	 

		if( pthread_create(&tid, NULL, do_work, (void *)new_message) != 0){
		    printf("cannot activate new thread \n");
		    fflush(stdout);
		    exit(EXIT_FAILURE);
		}
            #endif

	    #ifndef MULTITHREAD 
		do_work(&request_message);
            #endif
	}
    }/* end while */

    return 0;

}/* end main*/


