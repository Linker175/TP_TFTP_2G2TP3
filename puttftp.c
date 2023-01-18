#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUF_SIZE_DATA 516 // en-tete(4o) + data(512o max)

int main(int argc, char *argv[]) {
	
	char host[20], file[20];
	strcpy(host, argv[1]);
	strcpy(file, argv[2]);
	
	printf("Host : %s\n", host);
	printf("File : %s\n", file);
	
	int n_bytes;
	char file_content[BUF_SIZE_DATA - 4];
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
	
	// création requête WRQ 
	char buf[BUF_SIZE_DATA];
	memset(buf, 0, BUF_SIZE_DATA);
	buf[1] = 2; // mode écriture
	strcpy(buf + 2, argv[2]);
    size_t padding = strlen(buf+2);
    strcpy(buf + 2 + padding + 1, "octet");
    size_t padding2 = strlen(buf + 3 + padding);
    
    if ((n_bytes = sendto(sock, buf, padding + padding2 + 4, 0, res->ai_addr, res->ai_addrlen)) == -1) {
		perror("send WRQ");
		exit(EXIT_FAILURE);
	}
	printf("WRQ sent : OK\n");
	
	
	// Ouverture et lecture du fichier passé en argument
	int fd = open(file, O_RDONLY);
	if (fd == -1) {
		perror("Open file");
		exit(EXIT_FAILURE);
	}

	if ((n_bytes = read(fd, file_content, BUF_SIZE_DATA - 4)) == -1) {
		perror("read file");
		exit(EXIT_FAILURE);
	}
	
	// Création du paquet DATA
	memset(buf, 0, BUF_SIZE_DATA); 
	buf[1] = 3;
	buf[3] = 0; 
	strcpy(buf + 4, file_content);
	padding = strlen(buf + 4);
	
	// envoie DATA
	if (sendto(sock, buf, padding + 4, 0, res->ai_addr, res->ai_addrlen) == -1) {
		perror("Send DATA");
		exit(EXIT_FAILURE);
		}

	printf("Paquet DATA envoyé : ");
	for (int i = 0; i < 4; i++) {
		printf("%x", buf[i]);
	}
    for (int i = 4; i < padding + 4; i++) {
		printf("%c", buf[i]);
	}
	printf("\n");
	
	// Reception ACK
	if (recvfrom(sock, buf, 4, 0, res->ai_addr, &res->ai_addrlen) == -1) {
		perror("Receive ACK");
		exit(EXIT_FAILURE);
	}
	
	printf("ACK serveur : ");
	for (int i = 0; i < 4; i++) {
		printf("%x", buf[i]);
	}
	printf("\n");
	
	exit(EXIT_SUCCESS);
}
