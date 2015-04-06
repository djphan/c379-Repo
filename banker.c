#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h> 
#include <signal.h> /* Signals */
#include <string.h>
#include <sys/types.h>
     
#define INPUT_BUFFER 128

/*

matrix[process_count][resource_count]
for..
	for...
	set all to 0.

void release(int (*matrix)[resource_count])

to call release.

release(matrix);

*/

/* Protyping */
void exitHandler(int signum);

void exitHandler(int signum) 
{
	/* Handles ^C */
	printf("\nSimulation has ended. Goodbye :).\n");
	exit(0);
}

int main() 
{
	int numResources, resouceTok, *totalResources;
	int numProcesses, processTok, **totalProcess;
	char input[INPUT_BUFFER], *resourcestr;


	(void) signal(SIGINT, exitHandler); /* Initialize Handler */

	/* Input 1: Number of resources */
	for(;;) 
	{
		printf("\nInput the number of different resource types: ");
		// TODO: Note input 9fjdsljfdsoi gives out 9
		if (scanf("%d", &numResources) !=1 || numResources < 0  || numResources > 30)
		{
			printf("\nInvalid input. Please input a valid positive integer number < 30 .\n");
			/* Flush input buffer */
			while ( getchar() != '\n' );
		} 
		else if (numResources == 0)
		{
			printf("\nGave 0 resources. Invalid Number. Input a number > 0.\n");
			while ( getchar() != '\n' );
		}
		else 
		{
			printf("Inputted: %d resource types\n", numResources);
			while ( getchar() != '\n' );
			break;
		}
	}

	/* Create room for all the resource types */
	totalResources = calloc(numResources, sizeof(int));
	printf("Input the resource allocation: ");
	/* Input 2: Resource Allocation */
	for (int i=0; i<numResources; i++) 
	{
		for(;;)
		{
			if (scanf("%d", &resouceTok) !=1 || resouceTok < 0  || resouceTok > 30)
			{
				printf("\nInvalid input. Please input a valid positive integer number < 30 .\n");
				/* Flush input buffer */
				while ( getchar() != '\n' );
			} 
			else 
			{
				
				totalResources[i] = resouceTok;
				/* printf("Inputted: %d resource %d \n", totalResources[i], i+1); */
				break;
			}	
		
		}
		
	}

	/* Flush input buffer */
	while ( getchar() != '\n' );
	printf("All Resources Allocated\n");

	/* Input 3: Number of Processes */
	for(;;) {
		printf("Input the number of different processes: ");
		if (scanf("%d", &numProcesses) !=1 || numProcesses < 0  || numProcesses > 10)
		{
			printf("\nInvalid input. Please input a valid positive integer number < 10 .\n");
			while ( getchar() != '\n' );
		} 
		else if (numProcesses == 0)
		{
			printf("\nGave 0 processes. Invalid Number. Input a number > 0.\n");
			while ( getchar() != '\n' );
		}
		else 
		{
			printf("Inputted: %d different processes\n", numProcesses);
			while ( getchar() != '\n' );
			break;
		}
	}

	/* Create a matrix for the processes */
	/* Reference by totalProcess[Process #][Resource #] */
	totalProcess = malloc(numProcesses * sizeof(int *));
	for (int i = 0; i<numProcesses; i++)
	{
		totalProcess[i] = calloc(numResources, sizeof(int));
	}

	/* Input 4: Resource Allocation for Processes */
	for (int i=0; i<numProcesses; i++) 
	{ 
		printf("Input the number of resource for P%d: ", i+1);
		for (int j=0; j<numResources; j++) 
		{
			for(;;)
			{
				if (scanf("%d", &resouceTok) !=1 || resouceTok < 0  || resouceTok > 30)
				{
					printf("\nInvalid input. Please input a valid positive integer number < 30 .\n");
					/* Flush input buffer */
					while ( getchar() != '\n' );
				} 
				else 
				{
					totalProcess[i][j] = resouceTok;
					printf("Inputted: %d resource %d for process %d \n", 
							totalProcess[i][j], j+1, i+1); 
					break;
				}	
			
			}
			
		}

		/* Flush input buffer */
		while ( getchar() != '\n' );
	}

	
	return 0;
	exit(0);
}