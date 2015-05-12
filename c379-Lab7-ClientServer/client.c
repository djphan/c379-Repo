/* This client sends an integer value encoded as a string to an UDP server, and waits to receive the factorial of that number the Server  */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512
#define SERVERPORT 9930
#define PORT 9931
#define IP 2130706433  /* 127.0.0.1 */

int main( int argc, char ** argv)
{
	struct sockaddr_in si_me, si_other;
	int s, i, slen=sizeof(si_other);
	char buf[BUFLEN];

	if ( argc != 2 )
	{
		printf("\n\nusage: %s <int value>\n\n", argv[0]);
		return 3;
	}


	if ( ( s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 )
	{
		printf("Error in creating socket");
		return 1;
	}

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(IP);    /* htonl(INADDR_ANY) for any interface on this machine */

	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(SERVERPORT);
	si_other.sin_addr.s_addr = htonl(IP); 


	if ( bind(s, &si_me, sizeof(si_me)) == -1 )
	{
		printf("Error in binding the socket");
		return 2;
	}

	printf("\n\nClient listening to %s:%d\n\n", inet_ntoa(si_me.sin_addr), ntohs(si_me.sin_port));

	strcpy(buf, argv[1]);
	printf("\nSending %s to %s:%d\n", buf, inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	sendto(s, buf, strlen(buf) + 1, 0, &si_other, sizeof(si_other));

	if ( recvfrom(s, buf, BUFLEN, 0, &si_other, &slen) != -1)
		printf("\nReceived packet from %s:%d  Fact(%s): %s\n\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), argv[1], buf);

	close(s);
 	return 0;
 }
