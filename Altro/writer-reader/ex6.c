#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

char* r_file = "file.txt";
char* w_file = "write-file.txt";

int main(){

	FILE* rf, *wf;
	char* token, *line ;

	rf=fopen(r_file, "r");
	wf=fopen(w_file, "w+");

	while(fscanf(rf,"%m[^\n]", &line)!=EOF){
		
		fgetc(rf); // to consume "\n"

		token=strtok(line," ");

		while(token!=NULL){
			
			if (strlen(token)>6)
			       fprintf(wf,"%s ",token);

			token=strtok(NULL," ");	       
		
		}
	
	}

	free(token);
	free(line);

	exit(0);

}

