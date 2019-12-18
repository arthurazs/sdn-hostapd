#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define IP_DEST "10.0.1.1"
#define TCP_PORT 8080
#define BUFFER_SIZE 1024
#define MESSAGE_AUTH "GET /authenticated/%s/%s\r\n\r\n"
#define MESSAGE_DEAUTH "GET /deauthenticated/%s\r\n\r\n"

int socket_connect(char *host, in_port_t port){
	struct hostent *hp;
	struct sockaddr_in addr;
	int on = 1, sock;

	if((hp = gethostbyname(host)) == NULL){
		herror("gethostbyname");
		exit(1);
	}
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

	if(sock == -1){
		perror("setsockopt");
		exit(1);
	}

	if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("connect");
		exit(1);

	}
	return sock;
}

int authenticated(char mac[], char identity[])
{
    int fd;
	char buffer[BUFFER_SIZE];

    fd = socket_connect(IP_DEST, TCP_PORT);

    int total = strlen(MESSAGE_AUTH)+strlen(mac)+strlen(identity);
    char* data = (char*) malloc(total);

    sprintf(data, MESSAGE_AUTH, mac, identity);

	write(fd, data, strlen(data));
	bzero(buffer, BUFFER_SIZE);

	while(read(fd, buffer, BUFFER_SIZE - 1) != 0){
		fprintf(stderr, "%s", buffer);
		bzero(buffer, BUFFER_SIZE);
	}

    printf("\n");

	shutdown(fd, SHUT_RDWR);
	close(fd);

	return 0;
}

int deauthenticated(char mac[])
{
    int fd;
	char buffer[BUFFER_SIZE];

    fd = socket_connect(IP_DEST, TCP_PORT);

    int total = strlen(MESSAGE_DEAUTH)+strlen(mac);
    char* data = (char*) malloc(total);

    sprintf(data, MESSAGE_DEAUTH, mac);

	write(fd, data, strlen(data));
	bzero(buffer, BUFFER_SIZE);

	while(read(fd, buffer, BUFFER_SIZE - 1) != 0){
		fprintf(stderr, "%s", buffer);
		bzero(buffer, BUFFER_SIZE);
	}

    printf("\n");

	shutdown(fd, SHUT_RDWR);
	close(fd);

	return 0;
}
