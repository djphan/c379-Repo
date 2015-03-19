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
int serv_ip;
int port;
char *file_name;

const char argument_warning[] = "Improper use of arguments. "
								"Please use the following argument: \n"
								"./file_client <ip address> <port number to "
								"listen to> <file name> ";


int main(int argc, char * argv[]) 
{

	if (argc !=4) 
	{
		printf("%s", argument_warning);
		exit(0);
	} 
	else 
	{
		serv_ip = (int) inet_addr(argv[1]);
		printf("%d\n", serv_ip);
		port = atoi(argv[2]);
		file_name = argv[3];

	}
	exit(0);
}