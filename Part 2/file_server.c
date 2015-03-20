#include <arpa/inet.h> /* Convert IP Address String to Bytes */
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <signal.h> /* Signals */
#include <sys/socket.h> /* Sockets */
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h> /* Std In/Out */
#include <string.h>
#include <unistd.h> /* Misc Symbolic constants and types */
#include <errno.h>

/* Global Variables */
uint32_t localip = 2130706433;  /* 127.0.0.1 */
int port; 

const char argument_warning[] = "Improper use of arguments. "
								"Please use the following argument: \n"
								"./file_server <port number to listen to> "
								"<file serve path> <output log file path> ";

const char *sockerr = "Error in creating socket\n"; 
const char *binderr = "Error in binding the socket\n";
const char *listenerr = "Error in socket listening\n";

static void usage()
{
	extern char * __progname;
	fprintf(stderr, "Using Port: %s portnumber\n", __progname);
	exit(1);
}

static void kidhandler(int signum) {
	/* signal handler for SIGCHLD */
	waitpid(WAIT_ANY, NULL, WNOHANG);
}

int main(int argc, char * argv[]) 
{
	struct sockaddr_in server_socket, client_socket;
	int sersock, cli_len=sizeof(client_socket);

	char buffer[1024];

	struct sigaction sa;
	socklen_t clientlen;

	pid_t pid;
	u_long p;

	if (argc !=4) 
	{
		printf("%s", argument_warning);
		exit(0);
	} 
	else 
	{
		port = atoi(argv[1]);

		memset(&server_socket, 0, sizeof(server_socket));
		server_socket.sin_family = AF_INET;
		server_socket.sin_port = htons(port);
		server_socket.sin_addr.s_addr = htonl(localip);

		if ( ( sersock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 ) {
			printf("%s", sockerr);
			exit(1);
		}

		if (bind(sersock, (struct sockadder *) &server_socket, sizeof(server_socket)) == -1) {
			printf("%s", binderr);
			exit(1);
		}
		/*
		if (listen(sersock, 3) == -1) {
			printf("%s\n", listenerr);
			exit(1);
		}
		*/

		sa.sa_handler = kidhandler;
	    sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        
        if (sigaction(SIGCHLD, &sa, NULL) == -1)
        {
                err(1, "sigaction failed");
        }

		printf("File server setup and listening for connections on port: %d\n", port);

		/* Timer Code */

		while (1) 
		{
			if ( recvfrom(sersock, buffer, 1024, 0, &client_socket, &cli_len) != -1)
			{
				printf("\nReceived packet from %s:%d  Data: %s\n\n", 
					   inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port), buffer);
				buffer = 
				printf("\nSending Stuff: %s to %s:%d\n", buffer, inet_ntoa(client_socket.sin_addr), 
					   ntohs(client_socket.sin_port));

				sendto(sersock, buffer, strlen(buffer) + 1, 0, &client_socket, sizeof(client_socket));
			}
		}

		close(sersock);
	}	

}
