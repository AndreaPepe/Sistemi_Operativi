#include <unistd.h>
#include <stdio.h>

int main(int a ,char** b){

	uid_t id;

	id = getuid();		// User ID di chi lancia questo processo
	printf("I'm running on behalf of user %d\n",(int)id);
	printf(".. who would you like to become? ");
	scanf("%d",&id);
	setuid(id);		// da chiamare con "sudo" altrimenti non ho il potere di cambiare ID 
	id = getuid();
	printf("I'm now running on behalf of user %d\n",(int)id);
	pause();

}
