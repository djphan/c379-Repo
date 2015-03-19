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

/* Global Variables */
int localip = 2130706433;  /* 127.0.0.1 */
int port;
FILE *file; 
char *filepath;

/* Sockets */
struct sockaddr_in si_me, si_other;
int s, i, slen=sizeof(si_other);

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


void exitHandler(int signum) 
{
	/* Handles ^C */
	//printf("%s", c_catch);
	writeToPackets(file, filepath);
	exit(0);
}

int main(int argc, char * argv[]) 
{
	int src; 
	int dest; 
	int compareFlag = 0;
	char *pktString;

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

			if ( ( s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 )
			{
				printf("%s", sockerr);
				exit(0);
			}

			memset((char *) &si_me, 0, sizeof(si_me));
			si_me.sin_family = AF_INET;
			si_me.sin_port = htons(port);
			si_me.sin_addr.s_addr = htonl(localip); /* htonl(INADDR_ANY) for any interface on this machine */
			
			if ( bind(s, &si_me, sizeof(si_me)) == -1 )
			{
				printf("%s\n", binderr);
				exit(0);
			}			

			printf("pktgen listening to %s:%d\n\n", inet_ntoa(si_me.sin_addr), ntohs(si_me.sin_port));
		
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

				printf("\nSending: %s to %s:%d\n", pktString, inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
				sendto(s, pktString, (strlen(pktString) + 1), 0, &si_other, sizeof(si_other));

				if (pkt_counter % 20 == 0) {
					writeToPackets(file, filepath);
				}

				pkt_counter += 1;
			}
			close(s);
			exit(0);

		}
	}


}