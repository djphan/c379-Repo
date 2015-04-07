#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h> 
#include <signal.h> /* Signals */
#include <string.h>
#include <sys/types.h>
     
#define INPUT_BUFFER 128

/* Global Variables */
int sim_time = 1;

/* Protyping */
void exitHandler(int signum);

/* Signal Handler to end simulator */
void exitHandler(int signum) 
{
	/* Handles ^C */
	printf("\nSimulation has ended. Goodbye :).\n");
	exit(0);
}

int main() 
{
	for (int i =0; i<100; i++)
	{
		printf("%d\n", rand()%1);
	}

	exit(0);

	int numResources, resouceTok, *totalResources, *currentResources;
	int numProcesses, **totalProcess, **currentRequest, **currentAllocation, *processStatus;
	int actionflag = 0, waitflag = 0, releaseflag, releaseToken=0;

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
	currentResources = calloc(numResources, sizeof(int));

	/* Input 2: Resource Allocation */
	printf("Input the resource allocation: ");
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
				currentResources[i] = resouceTok;
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

	/* Keep track of process status 
	 * 0 = Unfilled/Wait 
	 * 1 = Complete
	 */ 
	processStatus = calloc(numProcesses, sizeof(int));

    /* Init this array to complete */
	for (int i=0; i<numProcesses; i++)
	{
		processStatus[i] = 1;
	}

	/* Create a matrix for the maximuprocesses */
	/* E.g. reference by totalProcess[Process #][Resource #] */

	totalProcess = malloc(numProcesses * sizeof(int *));
	currentRequest = malloc(numProcesses * sizeof(int *));
	currentAllocation = malloc(numProcesses * sizeof(int *));

	for (int i = 0; i<numProcesses; i++)
	{
		totalProcess[i] = calloc(numResources, sizeof(int));
		currentAllocation[i] = calloc(numResources, sizeof(int));
		currentRequest[i] = calloc(numResources, sizeof(int));
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
					/* printf("Inputted: %d resource %d for process %d \n", 
							totalProcess[i][j], j+1, i+1); */
					break;
				}	
			}
		}
		/* Flush input buffer */
		while ( getchar() != '\n' );
		printf("P%d allocated\n", i+1);
	}

	for(;;)
	{
		/* Every Sim_Time % 5 Execute Something */
		if (sim_time%5 == 0) 
		{
			/* Do something for each process */
			for (int i=0; i<numProcesses; i++)
			{
				actionflag = rand()%3;

				/* New Request */
				if ( (processStatus[i] == 1 && actionflag == 0) || (processStatus[i] == 1 && actionflag == 1) )
				{
					/* Exploit wonky way to print proper output to terminal */
					printf("Request for (");
					for (int j=0; j<numResources; j++)
					{
						currentRequest[i][j] = rand()%(totalProcess[i][j]+1);
						/* Output Formatting */
						if (j == numResources-1) {
							printf("%d", currentRequest[i][j]);
						} else {
							printf("%d,", currentRequest[i][j]);
						}
					}
					printf(") came from P%d\n", i+1);
					processStatus[i] = 0;
					sleep(2);

					for (int k=0; k<numResources; k++)
					{
						/* Check if you can allocate resources */
						if (currentRequest[i][k] <= currentResources[k])
						{
							continue;
						} else 
						{
							waitflag = 1;
							break;
						}
					}

					if (waitflag == 1) 
					{
						printf("Request for (");
						for (int l=0; l<numResources; l++)
						{
							/* Output Formatting */
							if (l == numResources-1) {
								printf("%d", currentRequest[i][l]);
							} else {
								printf("%d,", currentRequest[i][l]);
							}
							
						}
						printf(") from P%d cannot be satisfied, P%d is in waiting state\n", i+1, i+1);
				    	waitflag = 0;
				    	sleep(2);
					} 
					else 
					{
						printf("Request for (");
						for (int j=0; j<numResources; j++)
						{

							/* Output Formatting */
							if (j == numResources-1) {
								printf("%d", currentRequest[i][j]);
							} else {
								printf("%d,", currentRequest[i][j]);
							}
							
							/* Release all but 1 resource and set matrices accordingly */
							if (currentAllocation[i][j] == 0) {
								currentResources[j] = currentResources[j] - 1;
								currentAllocation[i][j] = 1;
								currentRequest[i][j] = 0;
							} 
							else 
							{
								currentResources[j] += currentAllocation[i][j] - 1;
								currentAllocation[i][j] = 1;
								currentRequest[i][j] = 0;
							}
							
						}
						printf(") from P%d has been granted\n", i+1);
						processStatus[i] = 1;
						/*Print Snapshot Here */
					}
				}

				/* Release logic. Must be allocated */
				else if (processStatus[i] == 0 && actionflag == 1)
				{
					/* Check if you can allocate resources */
					for (int k=0; k<numResources; k++)
					{
						if (currentRequest[i][k] <= currentResources[k])
						{
							continue;
						} 
						else 
						{
							waitflag = 1;
							break;
						}
					}

					if (waitflag == 0) 
					{
						printf("Request for (");
						for (int j=0; j<numResources; j++)
						{
							/* Output Formatting */
							if (j == numResources-1) {
								printf("%d", currentRequest[i][j]);
							} else {
								printf("%d,", currentRequest[i][j]);
							}

							/* Zero out request and keep track of current resources */
							currentResources[j] += currentAllocation[i][j] - 1;
							currentAllocation[i][j] = 1;
							currentRequest[i][j] = 0;
						}

						printf(") from P%d has been granted\n", i+1);

						/* Set flag to completed */
						processStatus[i] = 1;
						/*Print Snapshot Here */
					} 
					else 
					{
						/* Release Some Resources between 0 - Current Allocation-1 */
						waitflag = 0;

						printf("P%d has released (", i+1);
						for (int j=0; j<numResources; j++)
						{
							/* Check to handle pesky mod by 0 problem */
							if (currentAllocation[i][j] == 0) {
								releaseToken = 0;
							} else 
							{
								releaseToken = rand()%currentAllocation[i][j];
							}
							
							currentAllocation[i][j] = currentAllocation[i][j] - releaseToken;
							currentResources[j] += releaseToken;

							if (j < numResources-1) {
								printf("%d,", releaseToken);
							} 
							else 
							{
								printf("%d)", releaseToken);
							}

							releaseToken = 0;
						}	
						printf(" resources\n");
						releaseflag = 1;
					}					

				}	 
				/* Will do nothing */
				else
				{
					printf("P%d continues with no further request\n", i+1);
					sleep(2);
					continue;
				} 

				/* A release action occurred, check if new processes can be completed */
				if (releaseflag == 1) 
				{
					releaseflag = 0;

					for (int k=0; k<numResources; k++)
					{
						/* Check if you can allocate resources */
						if (currentRequest[i][k] <= currentResources[k])
						{
							continue;
						} 
						else 
						{
							waitflag = 1;
							break;
						}
					}

					if (waitflag == 1) 
					{
						/* Cant' Release */
				    	waitflag = 0;
				    	continue;
					} 
					else 
					{
						printf("Previous Request for (");
						for (int j=0; j<numResources; j++)
						{

							/* Output Formatting */
							if (j == numResources-1) {
								printf("%d", currentRequest[i][j]);
							} else {
								printf("%d,", currentRequest[i][j]);
							}

							currentResources[j] += currentAllocation[i][j] - 1;
							currentAllocation[i][j] = 1;
							currentRequest[i][j] = 0;

						}
						printf(") from P%d has been granted\n", i+1);
						processStatus[i] = 1;
						/*Print Snapshot Here */
					}
				}
			}
		}
		sim_time += 1;
	}
}