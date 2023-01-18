#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE_DATA 516 // en-tete(4o) + data(512o max)

int main(int argc, char *argv[]) {
	
	char host[20], file[20];
	strcpy(host, argv[1]);
	strcpy(file, argv[2]);
	
	printf("Host : %s\n", host);
	printf("File : %s\n", file);
	
	int n_bytes;
	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(struct addrinfo)); // Initialise à 0
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_UDP;
	
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
	
	// création requête RRQ 
	char buf[BUF_SIZE_DATA];
	memset(buf, 0, BUF_SIZE_DATA);
	buf[1] = 1;
	strcpy(buf + 2, argv[2]);
    size_t padding = strlen(buf+2);
    strcpy(buf + 2 + padding + 1, "octet");
    size_t padding2 = strlen(buf + 3 + padding);
    
    if (sendto(sock, buf, padding + padding2 + 4, 0, res->ai_addr, res->ai_addrlen) == -1) {
		perror("send RRQ");
		exit(EXIT_FAILURE);
	}
	
	n_bytes = BUF_SIZE_DATA;	
	
	while(n_bytes == BUF_SIZE_DATA) {
		
		// Première reception
		if ((n_bytes = recvfrom(sock, buf, BUF_SIZE_DATA, 0, res->ai_addr, &res->ai_addrlen)) == -1) {
			perror("receive DATA");
			exit(EXIT_FAILURE);
		}
			// En-tete
			printf("En-tête : ");
			for (int i = 0; i < 4; i++) {
				printf("%x", buf[i]);
			}
			printf("\nContenu du fichier : ");
			for (int i = 4; i < n_bytes; i++) {
				printf("%d", buf[i]);
			}
			printf("\n");
			
			// Envoie ACK
			buf[1] = 4;
			printf("Envoie ACK : ");
			for (int i = 0; i < 4; i++) {
				printf("%d", buf[i]);
			}
			printf("\n");
		}
		
		
		if (sendto(sock, buf, 4, 0, res->ai_addr, res->ai_addrlen) == -1) {
			perror("Send ACK");
			exit(EXIT_FAILURE);
		}
		printf("ACK send\n");

	
	close(sock);
	freeaddrinfo(res);
	
	exit(EXIT_SUCCESS);
}


	
	
