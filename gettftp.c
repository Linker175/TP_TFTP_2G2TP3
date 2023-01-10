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

	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(struct addrinfo)); // Initialise à 0
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_INET;
	hints.ai_protocol = 0;
	
	int en = getaddrinfo(host, "1069", &hints, &res);
	if (en == -1) {
		perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}
	
	int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
