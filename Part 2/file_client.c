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

/* Global Variables */
uint32_t serv_ip;
uint32_t localip = 2130706433;  /* 127.0.0.1 */
int port;
char *file_name;



const char argument_warning[] = "Improper use of arguments. "
								"Please use the following argument: \n"
								"./file_client <ip address> <port number to "
								"listen to> <file name> ";

const char *sockerr = "Error in creating socket\n"; 

int main(int argc, char * argv[]) 
{
	struct sockaddr_in client_socket, server_socket;
	int clisock, cli_len=sizeof(client_socket);
	char buffer[1024];
	struct timeval timeout;

	if (argc !=4) 
	{
		printf("%s", argument_warning);
		exit(0);
	} 
	else 
	{
		serv_ip =  (uint32_t) inet_addr(argv[1]);
		port = atoi(argv[2]);
		file_name = argv[3];
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000000;

		memset(&server_socket, 0, sizeof(server_socket));
		server_socket.sin_family = AF_INET;
		server_socket.sin_port = htons(port);
		server_socket.sin_addr.s_addr = htonl(serv_ip);

		memset(&client_socket, 0, sizeof(client_socket));
		client_socket.sin_family = AF_INET;
		client_socket.sin_port = htons(port);
		client_socket.sin_addr.s_addr = htonl(localip);

		if ( ( clisock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 ) {
			printf("%s", sockerr);
			exit(1);
		}

		if (setsockopt(clisock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
		{
			perror("Cannot Set Timeout\n");
			exit(1);
		}

		printf("\nSending File Request: %s to %s:%d\n", file_name, inet_ntoa(server_socket.sin_addr), 
					   ntohs(server_socket.sin_port));

		sendto(clisock, file_name, strlen(file_name) + 1, 0, &server_socket, sizeof(server_socket));

		printf("File client setup and listening for connections on port: %d\n", port);
		
		while (1) 
		{
			
			if (recvfrom(clisock, buffer, 1024, 0, &client_socket, &cli_len) != -1)
			{

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