#include <unistd.h>

int main(int x, char** y){
	char c;

	while(1){
		read(0,&c,1);	// canale di I/O (stdin), indirizzo, numero di byte
		write(1,&c,1);

	}
}
