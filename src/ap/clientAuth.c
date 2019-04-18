#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define TCP_PORT 10000
#define MAXDATASIZE 1024

int sendMessage(char hostname[], int port, char data[])
{
	int my_socket, bytes;
	char buffer[MAXDATASIZE];
	struct hostent *host_entry;
	struct sockaddr_in ip_address;

	if ((host_entry=gethostbyname(hostname)) == NULL)
	{
		herror("gethostbyname");
		exit(1);
	}
	if ((my_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	ip_address.sin_family = AF_INET;
	ip_address.sin_port = htons(port);
	ip_address.sin_addr = *((struct in_addr *) host_entry->h_addr);
	bzero(&(ip_address.sin_zero), 8);

	if (connect(my_socket, (struct sockaddr *)&ip_address, sizeof(struct sockaddr)) ==-1)
	{
		perror("connect");
		exit(1);
	}
	if(write(my_socket, data, strlen(data)) < 0)
	{
		printf("couldn't write\n");
		close(my_socket);
		exit(-1);
	}
	printf(">>> Sent %s to %s\n", data, hostname);

	if ((bytes=recv(my_socket, buffer, MAXDATASIZE, 0)) == -1)
	{
		perror("recv");
		exit(1);
	}

	buffer[bytes] = '\0';
	printf(">>> Got %s\n", buffer);
	close(my_socket);
	return 0;
}

int main_old(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: client hostname message\n");
		exit(1);
	}

	sendMessage(argv[1], TCP_PORT, argv[2]);

	return 0;
}
