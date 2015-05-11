#ifndef _memchunk_h
#define _memchunk_h

/* Header file for memchunk
 *
 * Includes the memchunk struct
 * and the function prototype of get_mem_layout
 */

struct memchunk 
{
	void *start;
	unsigned long length;
	int RW;    
};

int get_mem_layout(struct memchunk *chunk_list, int size);

#endif
