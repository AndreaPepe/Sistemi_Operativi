#include <unistd.h>

#define MAXBUF 4096

int main () {
	char buffer[MAXBUF];
	int res_r,res_w,prev_w;

	res_r = read(0, buffer, MAXBUF);

	while(res_r) {
		prev_w = 0;		// previous written # of bytes
		res_w = 0;		
		do {		// per evitare RESIDUI DI SCRITTURA
			prev_w =prev_w +res_w;
			res_w = write(1, &buffer[prev_w],res_r -prev_w); // scrive su STDOUT
		} while (res_w+prev_w <res_r);

 		res_r=read(0, buffer, MAXBUF);
	}

 	return 0;

}
