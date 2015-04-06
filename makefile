CC=gcc 
CFLAGS=-Wall -Wextra -m32 -std=c99

make all: banker.o banker

banker.o: banker.c banker.h
	$(CC) $(CFLAGS) -c banker.c

banker: banker.o
	$(CC) $(CFLAGS) -o banker banker.o

clean:
	-rm -rf *.o banker *.c~ *.h~ makefile~

tar:
	tar cvf 379djphan_a3.tar *.c *.h makefile