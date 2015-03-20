#include <arpa/inet.h> /* Convert IP Address String to Bytes */
#include <netinet/in.h>
#include <signal.h> /* Signals */
#include <sys/socket.h> /* Sockets */
#include <sys/types.h> 
#include <stdlib.h>
#include <stdio.h> /* Std In/Out */
#include <string.h>
#include <unistd.h> /* Misc Symbolic constants and types */
#include <errno.h>

/* Definitions */
#define LOCAL 2130706433  /* 127.0.0.1 */
#define MAXBUFF 65535

/* Global Variables */
int port;
char * rtTablePath;
char * stasticsFilePath;
FILE *rtFile;
FILE *statsFile;

int expPktNum = 0;
int unRoutPktNum = 0;
int delDirectNum = 0;
int routBNum = 0;
int routCNum = 0;

const char argument_warning[] = "Improper use of arguments. "
								"Please use the following argument: \n"
								"./router <port number to listen to> "
								"<routing table file path> <statistics "
								"file path> ";

const char *rtErr = "Error While Opening Routing Table";
const char *sockerr = "Error in creating socket\n";
const char *binderr = "Error in binding the socket\n"; 


/* RT Struct Definition */
struct rtable
{
	struct in_addr ipaddr;
	int bytes;
	char *destination;

};

/* Prototypes Definition */
void exitHandler(int signum);
void readRT(struct rtable *rtableValues); 

/* Reads in the RT Format: 
 * <IP Addr> <> <Destination Name>
 */
void readRT(struct rtable *rtableValues) 
{
	char rtContents[MAXBUFF], *IPdest, *bitNum, *destName;
	const char strParseChar[2] = " ";
	int counter = 0;

	/* Check Condition Until The End of RT_A.txt */
	while ( fgets(rtContents, MAXBUFF, rtFile)  != NULL)
	{
		/* Ignore blank lines */
		if (strlen(rtContents) < 7) 
		{
			continue;
		}
		
		/* String Parsing to Find */
		IPdest = strtok(rtContents, strParseChar);
		bitNum = strtok(NULL, strParseChar);
		destName = strtok(NULL, strParseChar);

		struct rtable entry = {
			inet_addr(IPdest), atoi(bitNum), destName
		};

		rtableValues[counter] = entry;
		counter+=1;
	}

}

int buildSocket (int port)
{
	int sock;
	struct sockaddr_in sockrouter; 

	if ( ( sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 )
	{
		printf("%s", sockerr);
		exit(0);
	}

	/* Socket Setup */
	memset((char *) &sockrouter, 0, sizeof(sockrouter));
	sockrouter.sin_family = AF_INET;
	sockrouter.sin_port = htons(port);
	sockrouter.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bind to specified socket */
	if (bind(sock, (struct sockaddr*) &sockrouter, sizeof(sockrouter)) == -1)
	{
		printf("%s", binderr);
		exit(1);
	}

	return sock;  
}


int main(int argc, char * argv[]) 
{
	// FIX DIS?
	struct rtable rtableValues[3];

	struct sockaddr_in sockpktgen;
	int sock, sockpktlength = sizeof(sockpktgen);
	char buff[MAXBUFF];

	if (argc !=4) 
	{
		printf("%s", argument_warning);
		exit(1);
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

		rtTablePath = argv[2];
		stasticsFilePath = argv[3];

		/* Read in Routing Table */
		rtFile = fopen(rtTablePath, "r");

		if (rtFile == NULL)
		{
			printf("%s\n", rtErr);
			exit(0);
		}

		readRT(rtableValues);
		printf(rtableValues[2].destination);

		fclose(rtFile);

		sock = buildSocket(port);
		
		while (1)
		{
			bzero(buff, MAXBUFF);

			if ( recvfrom(sock, buff, MAXBUFF, 0, 
				 (struct sockaddr *) &sockpktgen, &sockpktlength) != -1)
			{
				printf("Received packet from Port: %d. %s\n", 
						ntohs(sockpktgen.sin_port), buff);
			}

		}

		close(sock);

		}

}
