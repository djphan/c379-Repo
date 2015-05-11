#include <signal.h> /* Signals */
#include <setjmp.h> /* Signals */
#include <stdlib.h> /* Standard Lib */
#include <stdio.h> /* Std In/Out */
#include <unistd.h> /* Misc Symbolic constants and types */
#include "memchunk.h" /* Memchunk Header Declaration */

/* Prototyping */
void segSignalHandler(int signo); 

/* Global Variables */
static sigjmp_buf env; /* Buffer for sigjmp */

/* segSignalHandler 
 * Modified signal handler for the segfault signals
 * when accessing memory
 */
void segSignalHandler(int signo)
{
	siglongjmp(env, 1);
}

/* get_mem_layout
 * This takes in a memchunk and it's size and returns
 * the number of memory chunks is in the 'mem chunk'.
 *
 * A chunk is defined as a block of memory where the permissions
 * are the same. The block is measured in units of pages
 * for a 32 bit system.
 */

int get_mem_layout(struct memchunk *chunk_list, int size)
{
	unsigned long page_pos = 0; /* Current Page Position */
	int page_size = getpagesize(); /* Determine Page Size */
	unsigned long max_memnum = 0xFFFFFFFF; /* Maximum Memory Size */
	int num_chunks = 0; /* Number of chunks */

	char * cur_address = (char *) 0x00000000; /* Current Address */
	char * prev_address = (char *) 0x00000000; /* Previous Address */
	int cur_status = 3; /* Current Status Flag Initialized to 3 */
	int prev_status = 0; /* Previous Status Flag */
	char temp = 0; /* Temporary Placeholder for RW */
	
	(void) signal(SIGSEGV, segSignalHandler); /* Signal Handler for Seg Fault*/
	
	for(page_pos = 0; page_pos < max_memnum; page_pos += page_size) {
		if (cur_status != 3) {
			/* Keep track of previous state after first case */
			prev_status = cur_status;
			prev_address = cur_address;
		}

		cur_address = (char *) page_pos;
		if (cur_address < prev_address) {
			/* If we overflow, we need to break out of the loop */
			break;
		}

		if (!sigsetjmp(env, 1)) {
			/* Adjust status code as permissions are determined */
			cur_status = -1; 

			temp =  *cur_address; /* Test Read Permissions */
			cur_status = 0;

			*cur_address = temp; /* Test Write Permissions */
			cur_status = 1;

		} 

		if (num_chunks < size) {
			if (cur_status == prev_status) {
				/* Same permissions, add to current chunk length */
				chunk_list[num_chunks-1].length += page_size;
			} else {
				/* Different permissions. Add to chunk list */
				chunk_list[num_chunks].start = cur_address;
				chunk_list[num_chunks].length = page_size;
				chunk_list[num_chunks].RW = cur_status;
				num_chunks++;
			}
		} else {
			if (cur_status!=prev_status) {
				num_chunks++;
			}
		}
		
	}

	return num_chunks;
}
