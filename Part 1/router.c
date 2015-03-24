#include <arpa/inet.h> /* Convert IP Address String to Bytes */
#include <netinet/in.h>
#include <signal.h> /* Signals */
#include <sys/socket.h> /* Sockets */
#include <sys/types.h> 
#include <stdlib.h>
#include <stdint.h>
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

/* Packet Struct Definition */
typedef struct 
{
	int pkt_id;
	struct in_addr source_ip;
	struct in_addr dest_ip;
	int ttl;
	char *payload;
} Packet;

/* RT Struct Definition */
typedef struct 
{
	char *ipaddr;
	int bytes;
	char *destination;

} rtable;

/* Prototypes Definition */
void exitHandler(int signum);
void readRT(Packet packet) ;
int compereSubNet (int length, char *pktIP, char *destIP) ;
int buildSocket (int port);
char *trimwhitespace(char *str);

Packet reconstructPacket(char* buffer)
{
	const char charseperators[2] =", ";

	char *ipnum, *srcip, *destip, *ttl, *payload;
	ipnum = strtok(buffer, charseperators);
	srcip = strtok(NULL, charseperators);
	destip = strtok(NULL, charseperators);
	ttl = strtok(NULL, charseperators);
	payload = strtok(NULL, charseperators);

	Packet packet = { /* Create Packet Struct */
		atoi(ipnum),
		{ inet_addr(srcip) },
		{ inet_addr(destip) },
		atoi(ttl) - 1, /* Auto decrement */
		payload
	};
	
	return packet;
}

/* Reads in the RT Format: 
 * <IP Addr> <Bits> <Destination Name>
 */

void readRT(Packet packet) 
{
	char rtContents[MAXBUFF], *IPdest, *bitNum, *destName;
	const char strParseChar[2] = " ";
	int counter = 0;

	/* Check Condition Until The End of RT_A.txt */
	while ( fgets(rtContents, MAXBUFF, rtFile)  != NULL)
	{
		
		/* Skip Blank Lines */
		if (strlen(rtContents) < 7)
		{
			continue;
		}

		/* String Parsing to Find */
		IPdest = strtok(rtContents, strParseChar);
		bitNum = strtok(NULL, strParseChar);
		destName = trimwhitespace(strtok(NULL, strParseChar));

		if (compereSubNet( atoi(bitNum), inet_ntoa(packet.dest_ip), IPdest) == 1)
		{
			break;
		}
			
	}
	
	/* Invalid package completes the loop and returns a null*/
	if (strlen(rtContents) == 1)
	{		
		unRoutPktNum +=1;
		return;
	}

	/* StrChr logic structure for Router Numbers*/
	if (strcmp("0", destName) == 0)
	{
		delDirectNum += 1;
		return;
	}

	if (strcmp("RouterB", destName) == 0)
	{
		routBNum += 1;
		return;
	}

	if (strcmp("RouterC", destName) == 0)
	{
		routCNum += 1;
		return;
	}

}

/* Masking and Comparison to show correct dest */
int compereSubNet (int length, char *pktIP, char *destIP) 
{
	/* Make Mask */
	uint32_t mask = UINT32_MAX;
	mask = mask >> (32 - length);
	mask = mask << (32 - length);

	/* Network Order Shenanigans */
	uint32_t pktIPbits = htonl(inet_addr(pktIP));
	uint32_t destIPbits = htonl(inet_addr(destIP));

	uint32_t subnet = pktIPbits & mask;
	uint32_t masked_destnet = destIPbits & mask;

	if (subnet == masked_destnet) {
		return 1;
	} else {
		return 0;
	}
}


/* White space trimming function due to RT issues taken on StackOverflow
 * Solution provided by Adam Rosenfield
 * http://stackoverflow.com/questions/122616/
 * how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
 */
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}


void writeToStats(FILE *file, char * filepath) 
{
	file = fopen(filepath, "w+");
	fprintf(file, "Expired Packets: %d\n", expPktNum);
	fprintf(file, "Unroutable Packets: %d\n", unRoutPktNum);
	fprintf(file, "Delivered Directly (NetA): %d\n", delDirectNum);
	fprintf(file, "RouterB: %d\n", routBNum);
	fprintf(file, "RouterC: %d\n", routCNum);
	fclose(file);
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

void exitHandler(int signum) 
{
	/* Handles ^C */
	writeToStats(statsFile, stasticsFilePath);
	exit(0);
}


int main(int argc, char * argv[]) 
{
	Packet packet;
	struct sockaddr_in sockpktgen;
	int statscounter=0, sock, sockpktlength = sizeof(sockpktgen);
	char buff[MAXBUFF];

	if (argc !=4) 
	{
		printf("%s", argument_warning);
		exit(1);
	} 
	else 
	{
		(void) signal(SIGINT, exitHandler); /* Initialize Handler */
		port = atoi(argv[1]);


		/* Check for atoi problem arguments */
		if (port == 0)
		{
			printf("Improper port inputted (cannot be 0 or a string)\n");
			exit(1);
		}

		rtTablePath = argv[2];
		stasticsFilePath = argv[3];

		sock = buildSocket(port);

		
		while (1)
		{
			bzero(buff, MAXBUFF);

			if ( recvfrom(sock, buff, MAXBUFF, 0, 
				 (struct sockaddr *) &sockpktgen, &sockpktlength) != -1)
			{
				printf("Received packet from Port: %d. %s\n", 
						ntohs(sockpktgen.sin_port), buff);

				packet = reconstructPacket(buff);

				/* Condition 1 TTL */
				if (packet.ttl == 0)
				{
					expPktNum += 1;
					statscounter +=1;
					continue;
				}

				/* Read in Routing Table */
				rtFile = fopen(rtTablePath, "r");
				if (rtFile == NULL)
				{
					printf("%s\n", rtErr);
					exit(0);
				}
				readRT(packet);
				fclose(rtFile);
				statscounter += 1;

				if (statscounter % 20 == 0) 
				{
					writeToStats(statsFile, stasticsFilePath);
				}

	
			}

		}

		close(sock);

		}

}
