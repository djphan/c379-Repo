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

#define LOCAL 2130706433  /* 127.0.0.1 */

/* Global Variables */
int port;
char * rtTablePath;
char * stasticsFilePath;
FILE *rtFile;
FILE *statsFile;


struct sockaddr_in si_me, si_other;
int s, i, slen=sizeof(si_other);
char buf[128];

/* RT Struct Definition */
typedef struct 
{
	struct in_addr ipaddr;
	int bytes;
	char *destination;

} rtable;

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

/* Prototypes Definition */
void readRT(char *rtTablePath);

int main(int argc, char * argv[]) 
{

	char rtContents[128];
	const char strParseChar[2] = " ";
	char *strToken;


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

		rtTablePath = argv[2];
		stasticsFilePath = argv[3];

		/* Read in Routing Table */
		rtFile = fopen(rtTablePath, "r");

		if (rtFile == NULL)
		{
			printf("%s\n", rtErr);
			exit(0);
		}

		int counter = 0;

		/* Check Condition Until The End of RT_A.txt */
		while ( fgets(rtContents, 128, rtFile)  != NULL)

		{
			// TODO: Check this?
			if (strlen(rtContents) < 3) 
			{
				continue;
			}

			//printf("%d\n", counter);
			printf("%s", rtContents);
			//printf("Str Len: %lu\n", strlen(rtContents));

			/* String Parsing to Find */
			strToken = strtok(rtContents, strParseChar);
			while (strToken != NULL) 
			{
				printf("%s\n", strToken);
				strToken = strtok(NULL, strParseChar);
			}


			counter+=1;
		}


		fclose(rtFile);

		if ( ( s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 )
		{
			printf("%s", sockerr);
			exit(0);
		}

		memset((char *) &si_me, 0, sizeof(si_me));
		si_me.sin_family = AF_INET;
		si_me.sin_port = htons(port);
		si_me.sin_addr.s_addr = htonl(LOCAL);    /* htonl(INADDR_ANY) for any interface on this machine */

		si_other.sin_family = AF_INET;
		si_other.sin_port = htons(port);
		si_other.sin_addr.s_addr = htonl(LOCAL); 

		printf("\n\nRouter listening to %s:%d\n\n", inet_ntoa(si_me.sin_addr), ntohs(si_me.sin_port));
		strcpy(buf, argv[1]);
		printf("\nSending %s to %s:%d\n", buf, inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		sendto(s, buf, strlen(buf) + 1, 0, &si_other, sizeof(si_other));

		if ( recvfrom(s, buf, strlen(buf), 0, &si_other, &slen) != -1)
			printf("\nReceived packet from %s:%d  Fact(%s): %s\n\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), argv[1], buf);

		close(s);

		}

}
