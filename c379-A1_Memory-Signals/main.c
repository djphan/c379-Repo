#include <signal.h> /* Signals */
#include <setjmp.h> /* Set Jump */
#include <stdio.h> /* Std In/Out */
#include <unistd.h> /* Misc Symbolic constants and types */
#include <errno.h> /* Error Codes */

#include "memchunk.h" /* Memchunk */

int const CHUNK_LIST_SIZE = 20;

int main() {
	printf("Starting Test Script...\n");

	printf("Making Chunk List ...\n");
	struct memchunk chunk_list[CHUNK_LIST_SIZE];
	int i = 0;

	for (i = 0; i < CHUNK_LIST_SIZE; i++) {
		chunk_list[i].start = NULL;
		chunk_list[i].length = 0;
		chunk_list[i].RW = 3;
	}
	printf("Initalized Chunk List...\n");

	int chunk_num = get_mem_layout(chunk_list, CHUNK_LIST_SIZE);

	for (i = 0; i < CHUNK_LIST_SIZE; i++) {
		if (i >= chunk_num) {
			break;
		}

		printf("Chunk #: %d. ", i);
		printf("Starting at: %p. ", (void*) chunk_list[i].start);
		printf("Chunk Size: %lu. ", chunk_list[i].length);
		printf("Chunk RW: %d. \n", chunk_list[i].RW);
	}
		printf("Total Number of Chunks : %d.\n", chunk_num);

	return 0;
}
