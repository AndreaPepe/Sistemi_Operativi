#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){
	char* heap_buff;
	
	printf("Please insert a line:\n");
	fflush(stdout);		
	
	scanf("%m[^\n]", &heap_buff);

	char stack_buff[strlen(heap_buff)];
	strcpy(stack_buff, heap_buff);
	
	free(heap_buff);

	printf("[Stack buffer]: %s\n",stack_buff);
	return 0;
}
