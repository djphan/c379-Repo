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

#define MAXBUFF 1024

/* Global Variables */
char *file_name;

const char *dollar = "$";
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

	/* Socket Setup */
	memset(&client_socket, 0, sizeof(client_socket));
	client_socket.sin_family = AF_INET;
	client_socket.sin_port = htons(port);
	client_socket.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Don't bind in client. Just send/recv */

	return clisock;	

}

int main(int argc, char * argv[]) 
{
	struct sockaddr_in server_socket;
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
		server_socket.sin_port = htons(port);

		/* Sets address of server from arguments */
		inet_aton(argv[1], &server_socket.sin_addr);

		clisock = buildSocket(port);

		if (setsockopt(clisock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
		{
			perror("Cannot Set Timeout\n");
			exit(1);
		}

		printf("\nSending File Request: %s to %s:%d\n", file_name, inet_ntoa(server_socket.sin_addr), 
					   ntohs(server_socket.sin_port));

		sendto(clisock, file_name, (strlen(file_name) + 1), 0, 
						(struct sockaddr*) &server_socket, sizeof(server_socket));

		printf("File client setup and listening for connections on port: %d\n", port);
		
		while (1) 
		{
			bzero(buffer, MAXBUFF);

			if ( recvfrom(clisock, buffer, MAXBUFF, 0, 
				 (struct sockaddr *) &server_socket, &sersocklen) != -1 )
			{

				if ( (strlen(buffer) == 1) && ( strchr(dollar, *buffer) ) ) 
				{
					/* Find Dollar Sign and Ignore It. */
					printf("\n");
					// Do something here
					//exit(1);
				} else {
					buffer[MAXBUFF-1] = 0;
					printf("%s", buffer);
				}

			} 
			else 
			{
				printf("\n\nTimeout\n");
				
				//sendto(clisock, file_name, (strlen(file_name) + 1), 0, 
										//(struct sockaddr*) &server_socket, sizeof(server_socket));
				exit(1);
			}
			
		}

		close(clisock);

	}
	exit(0);
}
