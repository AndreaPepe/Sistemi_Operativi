#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 1024

int main( int argc, char** argv){

	if(argc!=2){
		printf("Error! Usage: ./a.out <string>\n");
		exit(-1);
	}

	if(strlen(argv[1]) > SIZE){
		printf("Error! <string> must have at least made of %d characters\n", SIZE);
		exit(-1);
	}
	
	printf("Input string acquired succesfully\n\n");
	fflush(stdout);

	char buffer[SIZE];
	strcpy(buffer, argv[1]);
	int len = strlen(buffer);

	printf("Reverse printing: ");
	for(int i=len; i>=0; i--)
		printf("%c", buffer[i]);

	printf("\n\n");

	printf("Printing: %s\n", buffer);
	return 0;

}
