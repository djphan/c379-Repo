CC=gcc
CFLAGS=-Wall -Wextra -m32

all: file_server file_client

file_server: file_server.c
	$(CC) $(CFLAGS) -o file_server file_server.c

file_client: file_client.c
	$(CC) $(CFLAGS) -o file_client file_client.c

clean:
	-rm -rf *.o file_server file_client *.out 

tar:
	tar cvf 379_a2p2.tar *.c *.h makefile*