#include <arpa/inet.h> /* Convert IP Address String to Bytes */
#include <netinet/in.h>
#include <signal.h> /* Signals */
#include <sys/socket.h> /* Sockets */
#include <sys/time.h>  
#include <sys/types.h> 
#include <stdlib.h>
#include <stdio.h> /* Std In/Out */
#include <string.h>
#include <unistd.h> /* Misc Symbolic constants and types */
#include <errno.h>

#define SERVERPORT 9999
#define MAXBUFF 65535

/* Global Variables */

char *file_name;

const char argument_warning[] = "Improper use of arguments. "
								"Please use the following argument: \n"
								"./file_client <ip address> <port number to "
								"listen to> <file name> ";

const char *sockerr = "Error in creating socket\n";
const char *binderr = "Error in binding the socket\n";

/* Prototyping */
int buildSocket (int port);

int buildSocket (int port) {
	int clisock;
	struct sockaddr_in client_socket;

	if ( ( clisock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 ) {
		printf("%s", sockerr);
		exit(1);
	}

	memset(&client_socket, 0, sizeof(client_socket));
	client_socket.sin_family = AF_INET;
	client_socket.sin_port = htons(port);
	client_socket.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(clisock, (struct sockaddr *) &client_socket, sizeof(client_socket)) == -1)
	{
	printf("%s", binderr);
	exit(1);
	} 

	return clisock;	

}

int main(int argc, char * argv[]) 
{
	uint32_t serv_ip;

	struct sockaddr_in server_socket, client_socket;
	int port, clisock, sersocklen = sizeof(server_socket);
	char buffer[MAXBUFF];
	struct timeval timeout;

	if (argc !=4) 
	{
		printf("%s", argument_warning);
		exit(0);
	} 
	else 
	{

		serv_ip = inet_addr(argv[1]);
		port = atoi(argv[2]);

		/* Check for atoi problem arguments */
		if (port == 0)
		{
			printf("Improper port inputted (cannot be 0 or a string)\n");
			exit(1);
		}

		file_name = argv[3];

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		/* Set the values of the sockets */
		memset(&server_socket, 0, sizeof(server_socket));
		server_socket.sin_family = AF_INET;
		server_socket.sin_port = htons(SERVERPORT);
		server_socket.sin_addr.s_addr = serv_ip;

		clisock = buildSocket(port);

		if (setsockopt(clisock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
		{
			perror("Cannot Set Timeout\n");
			exit(1);
		}

		printf("\nSending File Request: %s to %s:%d\n", file_name, inet_ntoa(server_socket.sin_addr), 
					   ntohs(server_socket.sin_port));

		sendto( clisock, file_name,(strlen(file_name) + 1), 0, 
				&server_socket, &sersocklen);

		printf("File client setup and listening for connections on port: %d\n", port);
		
		while (1) 
		{
			printf("hihihi\n");
			if ( recvfrom(clisock, buffer, MAXBUFF, 0, 
				 (struct sockaddr *) &server_socket, &sersocklen) != -1 )
			{
				sendto(clisock, file_name, strlen(file_name) + 1, 0, &server_socket, sizeof(server_socket));
			} 
			else 
			{
				printf("Timeout\n");
				exit(1);
			}
			
		}

		close(clisock);

	}
	exit(0);
}
