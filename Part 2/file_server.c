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

#define LOCAL 2130706433  /* 127.0.0.1 */
#define CHUNK 1024
#define MAXBUFF 65535

/* Global Variables */
const char *dollar = "$";
const char argument_warning[] = "\nImproper use of arguments. "
								"Please use the following argument: \n"
								"./file_server <port number to listen to> "
								"<file serve directory path> "
								"<output log file directory path> ";

const char *sockerr = "\nError in creating socket\n"; 
const char *binderr = "\nError in binding the socket\n";
const char *listenerr = "\nError in socket listening\n";

/* Prototyping */
static void kidhandler(int signum);
int buildSocket (int port);

/* Handler to prevent zombies */
static void kidhandler(int signum) {
	waitpid(WAIT_ANY, NULL, WNOHANG);
}


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
	server_socket.sin_port = htons(port);
	server_socket.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sersock, (struct sockaddr *) &server_socket, sizeof(server_socket)) < 0) 
	{
		printf("%s\n", strerror(errno));
		printf("%s", binderr);
		exit(1);
	}

	return sersock;
}

int main(int argc, char * argv[]) 
{
	struct sockaddr_in client_socket;
	int port, sersock, cli_len=sizeof(client_socket), pathSize, lastbuffer;
	int daeid, logcounter=0;
	char buffer[MAXBUFF], *filepath, *dirpath, *badpath;
	char fileBuffer[CHUNK], *logpath;

	FILE *sendFile, *logFile;

	/* Process information */
	struct sigaction sa;
	pid_t pid, sid;

	if (argc !=4) 
	{
		printf("%s", argument_warning);
		exit(0);
	} 
	else 
	{
		port = atoi(argv[1]);
		dirpath = argv[2];
		logpath = argv[3];

		/* Check for atoi problem arguments */
		if (port == 0)
		{
			printf("Improper port inputted (cannot be 0 or a string)\n");
			exit(1);
		}

		/* Taken from Lab6server.c
	 	* first, let's make sure we can have children without leaving
	 	* zombies around when they die - we can do this by catching
	 	* SIGCHLD.
	 	*/	
		sa.sa_handler = kidhandler;
	    sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        
        if (sigaction(SIGCHLD, &sa, NULL) == -1)
        {
                err(1, "sigaction failed");
        }
	
		sersock = buildSocket(port);
	
		/* There be daemons in my code */
		//daeid = daemon(1,1);
		//if (daeid == -1) 
		//{
		//	printf("Daemonizing failed.\n");
		//	exit(1);
		//}

		printf("File server setup and listening for connections on port: %d\n", port);
		while (1) 
		{
			pid = fork();
			if (pid == -1)
			{
				err(1, "Fork'd failed");
				exit(1);
			}

			while (1) 
			{
				if (pid != 0) 
				{ 

				/* Parent Process Doesnt Do Anything */
				continue;
 
				} 
				else 
				{ 
					/* Child Slave Process Does All The Work */
					bzero(buffer, MAXBUFF);
					bzero(fileBuffer, CHUNK);

					if ( recvfrom(sersock, buffer, MAXBUFF, 0, (struct sockaddr *) &client_socket, &cli_len) != -1 )
					{
						/* Can't abstract to function ??  Create file path */
						pathSize = (snprintf(NULL, 0, "%s/%s", dirpath, buffer) + 1);
						filepath = malloc(pathSize);
						snprintf(filepath, pathSize, "%s/%s", dirpath, buffer);

						sendFile = fopen(filepath, "r");

						if (sendFile == 0) {
							/* Let Client Know They Have Mistake :) */
							badpath = "Given file name does not exist, please resend request. \n";
							printf("%s", badpath);
							sendto(sersock, badpath, (strlen(badpath)), 0, 
									(struct sockaddr*) &client_socket, sizeof(client_socket));
							continue;

						}
				
						lastbuffer = fread(fileBuffer, 1, CHUNK, sendFile);

						while (lastbuffer == CHUNK) 
						{
							/* Send all 1024 bytes as a full chunk and clear buffer */
							sendto(sersock, fileBuffer, (strlen(fileBuffer)), 0, 
									(struct sockaddr*) &client_socket, sizeof(client_socket));
							bzero(fileBuffer, CHUNK);
		 					lastbuffer = fread(fileBuffer, 1, CHUNK, sendFile);					
						}

						if (lastbuffer % CHUNK == 0) 
						{
							/* Exact %1024 */
							sendto(sersock, dollar, (strlen(dollar)), 0, 
									(struct sockaddr*) &client_socket, sizeof(client_socket));
							bzero(fileBuffer, CHUNK);
							fclose(sendFile);
						} 
						else 
						{
							sendto(sersock, fileBuffer, (strlen(fileBuffer)), 0, 
									(struct sockaddr*) &client_socket, sizeof(client_socket));
							bzero(fileBuffer, CHUNK);
							fclose(sendFile);
						}

						printf("Sent over file at %s to client\n", filepath);

						logFile = fopen(logpath, "a+");
						while (logFile == 0) 
						{
							/* Does its best to open a log file */
							printf("Log file cannot be created. Attempting again\n");
							if (logCounter == 100) 
							{
								printf("Log file cannot be created. Giving Up\n");
								exit(0);
							}

							logCounter += 1;
							logFile = fopen("log", "a+");
						}
						flock(logFile, LOCKEX);
						//writeToLog();
						flock(logFile, LOCKUN);
						fclose(logFile);

						break;

					}
				}
			}

		break;
		}
		

		close(sersock);
	}	

}
