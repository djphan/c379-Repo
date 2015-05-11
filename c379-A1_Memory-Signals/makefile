CC=gcc
CFLAGS=-Wall -Wextra -m32

memchunk.o: memchunk.c memchunk.h
	$(CC) $(CFLAGS) -c memchunk.c

test.o: test.c memchunk.h
	$(CC) $(CFLAGS) -c test.c 

test: memchunk.o test.o 
	$(CC) $(CFLAGS) -o test memchunk.o test.o 

clean:
	-rm -rf *.o *.c~ *.h~ makefile~ test

tar:
	tar cvf djphan_379_a1.tar *.c *.h makefile*
