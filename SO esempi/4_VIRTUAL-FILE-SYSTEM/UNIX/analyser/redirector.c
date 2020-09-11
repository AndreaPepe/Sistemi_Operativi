#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char *argv[]) {

  int fd;

  if (argc!= 2) {
    printf("Syntax: write_on_file <file_name>\n");
    exit(-1);
  }

  fd=open(argv[1], O_CREAT| O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR);

  if (fd== -1) {
    perror("Open error: ");
    exit(-2);
  }

  printf("fd=%d\n",fd);
  close (1);		//chiude lo STDOUT
  dup(fd);		// il file descriptor diventa 1 al posto dello STDOUT
  execve("./writer", NULL, NULL);
  perror("Exec error: ");
  exit(-3);
}

/*  RIDIREZIONA LA SCRITTURA DALLO STDOUT AL FILE APERTO  */
