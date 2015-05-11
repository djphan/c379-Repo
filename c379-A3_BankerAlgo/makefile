CC=gcc 
CFLAGS=-Wall -Wextra -m32 -std=c99

make all: banker

banker: banker.c
	$(CC) $(CFLAGS) -o banker banker.c

clean:
	-rm -rf *.o banker *.c~ *.h~ makefile~

tar:
	tar cvf 379djphan_a3.tar *.c *.h makefile