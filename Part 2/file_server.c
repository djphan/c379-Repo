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

#define CHUNK 1024
#define MAXBUFF 65535

/* Global Variables */
const char argument_warning[] = "Improper use of arguments. "
								"Please use the following argument: \n"
								"./file_server <port number to listen to> "
								"<file serve path> <output log file path> ";

const char *sockerr = "Error in creating socket\n"; 
const char *binderr = "Error in binding the socket\n";
const char *listenerr = "Error in socket listening\n";

/* Prototyping */
static void kidhandler(int signum);
int buildSocket (int port);

/*
static void usage()
{
	extern char * __progname;
	fprintf(stderr, "Using Port: %s portnumber\n", __progname);
	exit(1);
}
*/

/* Handler to prevent zombies */
/*
static void kidhandler(int signum) { */
	/* signal handler for SIGCHLD *//*
	waitpid(WAIT_ANY, NULL, WNOHANG);
}
*/
int buildSocket (int port)
{
	struct sockaddr_in server_socket; 
	int sersock;

	if ( ( sersock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 ) 
	{
		printf("%s", sockerr);
		exit(1);
	}

	/* Set the server sockets */
	memset(&server_socket, 0, sizeof(server_socket));
	server_socket.sin_family = AF_INET;
	server_socket.sin_port = htons(9999);
	server_socket.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sersock, (struct sockaddr *) &server_socket, sizeof(server_socket)) < 0) 
	{
		printf("%s", binderr);
		exit(1);
	}

	return sersock;
}

int main(int argc, char * argv[]) 
{
	struct sockaddr_in client_socket;
	int port, sersock, cli_len=sizeof(client_socket);

	char buffer[MAXBUFF];
	/*
	struct sigaction sa;

	pid_t pid;
	u_long p;
	*/

	if (argc !=4) 
	{
		printf("%s", argument_warning);
		exit(0);
	} 
	else 
	{

		port = atoi(argv[1]);


		/* Check for atoi problem arguments */
		if (port == 0)
		{
			printf("Improper port inputted (cannot be 0 or a string)\n");
			exit(1);
		}


		sersock = buildSocket(port);


		/* Define Client Sock */
		client_socket.sin_family = AF_INET;
		client_socket.sin_port = htons(port);
		client_socket.sin_addr.s_addr = htonl(INADDR_ANY);

		/*
		if (listen(sersock, 3) == -1) {
			printf("%s\n", listenerr);
			exit(1);
		}
		


		sa.sa_handler = kidhandler;
	    sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        
        if (sigaction(SIGCHLD, &sa, NULL) == -1)
        {
                err(1, "sigaction failed");
        }
		*/
		printf("File server setup and listening for connections on port: %d\n", port);
		
		while (1) 
		{
			if ( recvfrom(sersock, buffer, MAXBUFF, 0, (struct sockaddr *) &client_socket, &cli_len) != -1 )
			{
				printf("%s", buffer);	
			}
		}
		

		close(sersock);
	}	

}
