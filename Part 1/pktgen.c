#include <arpa/inet.h> /* Convert IP Address String to Bytes */
#include <netinet/in.h>
#include <signal.h> /* Signals */
#include <sys/socket.h> /* Sockets */
#include <sys/time.h>  
#include <sys/types.h>
#include <sys/wait.h> 
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
FILE *file; 
char *filepath;

const char argument_warning[] = "Improper Number of arguments." 
								"Please supply this command: \n"
								"./pktgen "
								"<port number to router> "
								"<packets file path> \n";

const char *file_err = "File could not be opened\n";
const char *sockerr = "Error in creating socket\n"; 
const char *binderr = "Error in binding the socket\n";
const char *c_catch = "Terminating with ^ C\n";
const char *compar_err ="Comparison error\n";

static int pkt_counter = 1;
static int numAtoB = 0;
static int numAtoC = 0;
static int numBtoA = 0;
static int numBtoC = 0;
static int numCtoA = 0;
static int numCtoB = 0;
static int numInvalid = 0;

char *payloadString = "Don\'t Panic";

/* Protyping */
void exitHandler(int signum);
char *ipPicker(int pktType);
int compareSrcDest (int src, int dest);
void writeToPackets(FILE *file, char * filepath);
int buildSocket (int port);

/* Packet Struct Definition */
typedef struct 
{
	int pkt_id;
	struct in_addr source_ip;
	struct in_addr dest_ip;
	int ttl;
	char *payload;
} Packet;

/* Function to generate random IPs for Host and Dest */
char * ipPicker(int pktType) 
{
	char * buffer;
	int size;

	/* Defined IPs for Host in Assignments */
	char * hostA = "192.168.128";
	char * hostB = "192.168.192";
	char * hostC = "192.224";
	char * hostD = "168.130.192.01";

	/* Note the last byte, or two bytes need to be auto generated */	
	
	if ( pktType == 0 )
	{
		/* Calculate size of string to malloc */
		size = snprintf(NULL, 0, "%s.%d", hostA, (int)rand()%256) + 1; 
		buffer = (char *) malloc(size);

		/* snprintf concatenates strings used to generate our IP*/
		snprintf(buffer, size,  "%s.%d", hostA, (int)rand()%256);
	}
	else if ( pktType == 1 )
	{
		size = snprintf(NULL, 0, "%s.%d", hostB, (int)rand()%256) + 1;
		buffer = (char *) malloc(size);
		snprintf(buffer, size, "%s.%d", hostB, (int)rand()%256);
	}	
	else if ( pktType == 2 )
	{
		size = snprintf(NULL, 0, "%s.%d.%d", 
					hostC, (int)rand()%256, (int)rand()%256) + 1;

		buffer = (char *) malloc(size);

		snprintf(buffer, size, "%s.%d.%d", 
					hostC, (int)rand()%256, (int)rand()%256);

	}
	else
	{
		/* Invalid Case */
		size = snprintf(NULL, 0, "%s", hostD) +1;
		buffer = malloc(size);
		snprintf(buffer, size, "%s", hostD);
	}
	
	return buffer;
}

int compareSrcDest(int src, int dest) 
{
	/* Giant If/Else Switch Cases to Handle Cases */
	if (dest == 3) {
		numInvalid += 1;
	}
	else if (src == 0 & dest == 1) 
	{
		numAtoB += 1;
	} 
	else if  (src == 0 & dest == 2)
	{ 
		numAtoC += 1;
	}

	else if (src == 1 & dest == 0) 
	{
		numBtoA += 1;
	}
	else if (src == 1 & dest == 2) 
	{
		numBtoC += 1;
	}
	else if (src == 2 & dest == 0) 
	{
		numCtoA += 1;
	}
	else if (src == 2 & dest == 1) 
	{
		numCtoB += 1;
	} 
	else 
	{
		printf("You shouldn't get here :(");
		return -1;
	}

	return 0;
}

/* Handles returning a valid destination */
int findDest(int src) 
{
	int dest = (int) rand()%4;

	while (1) 
	{
		if (dest == src) 
		{
			dest = (int) rand()%4;
			continue;
		} 
		else 
		{
			return dest;
		}
	}

}

void writeToPackets(FILE *file, char * filepath) 
{
	fopen(filepath, "w+");
	fprintf(file, "NetA to NetB: %d\n", numAtoB);
	fprintf(file, "NetA to NetC: %d\n", numAtoC);
	fprintf(file, "NetB to NetA: %d\n", numBtoA);
	fprintf(file, "NetB to NetC: %d\n", numBtoC);
	fprintf(file, "NetC to NetA: %d\n", numCtoA);
	fprintf(file, "NetC to NetB: %d\n", numCtoB);
	fprintf(file, "Invalid Destination: %d\n", numInvalid);
	fclose(file);
}

char * pktStringMaker(Packet packet, char* pktString) 
{
	int pktStringsize = snprintf(NULL, 0, 
						"%d, %s, %s, %d, %s", 
						packet.pkt_id, 
						inet_ntoa(packet.source_ip), 
						inet_ntoa(packet.dest_ip), 
						packet.ttl, packet.payload) +1;

	pktString = malloc(pktStringsize);

	snprintf(pktString, pktStringsize, 
			"%d, %s, %s, %d, %s", packet.pkt_id, 
			inet_ntoa(packet.source_ip), 
			inet_ntoa(packet.dest_ip), 
			packet.ttl, packet.payload );

	return pktString;
}

int buildSocket (int port) 
{
	int sock;
	struct sockaddr_in sockpktgen;

	if ( ( sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) < 0 )
	{
		printf("%s", sockerr);
		exit(0);
	}

	/* Set Socket Paramters */
	memset((char *) &sockpktgen, 0, sizeof(sockpktgen));
	sockpktgen.sin_family = AF_INET;
	sockpktgen.sin_port = htons(9999);
	sockpktgen.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr *) &sockpktgen, sizeof(sockpktgen)) == -1)
	{
		printf("%s", binderr);
		exit(1);
	} 		

	printf("pktgen open and bind socket on port %d\n\n",  
			ntohs(sockpktgen.sin_port));

	return sock;
}

void exitHandler(int signum) 
{
	/* Handles ^C */
	//printf("%s", c_catch);
	writeToPackets(file, filepath);
	exit(0);
}

int main(int argc, char * argv[]) 
{
	int src, dest, compareFlag = 0;
	char *pktString;

	/* Sockets */
	struct sockaddr_in sockrouter;
	int sock;

	(void) signal(SIGINT, exitHandler); /* Initialize Handler */

	/* CMD Line Arguments */
	if (argc != 3)	 
	{
		printf("%s", argument_warning); /* Argument Error */
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

		filepath = argv[2];
		file = fopen(filepath, "w+");
	
		if (file == 0) 
		{
			printf("%s", file_err); /* File Creation Error */
			exit(0);
		} 

		else 
		{
			fclose(file);
			sock = buildSocket(port);

			/* Define Client Sock */
			sockrouter.sin_family = AF_INET;
			sockrouter.sin_port = htons(port);
			sockrouter.sin_addr.s_addr = htonl(INADDR_ANY);

			while(1) 
			{
				/* From a2 specs, source is from A, B or C */
				src = (int)rand()%3;
				dest = findDest(src);

				Packet packet = { /* Create Packet Struct */
					pkt_counter,
					{ inet_addr(ipPicker(src)) },
					{ inet_addr(ipPicker(dest)) },
					rand()%4 + 1, /* TTL */
					payloadString
				};

				pktString = pktStringMaker(packet, *pktString);
				compareFlag = compareSrcDest(src, dest);

				if (compareFlag == -1) 
				{
					printf("%d", compareFlag);
					exit(0);
				}

				printf("Sending: %s to Port: %d\n", pktString, 
					ntohs(sockrouter.sin_port));

				if (sendto(sock, pktString, (strlen(pktString) + 1), 0, 
						(struct sockaddr*) &sockrouter, sizeof(sockrouter)))
				{
					if (pkt_counter % 20 == 0) {
						writeToPackets(file, filepath);
					}

					pkt_counter += 1;
					sleep(2);
				}

			}
			close(sock);
			exit(0);

		}
	}


}
