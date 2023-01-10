#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
	
	char host[20], file[20];
	strcpy(host, argv[1]);
	strcpy(file, argv[2]);
	
	printf("Host : %s\n", host);
	printf("File : %s\n", file);

}
