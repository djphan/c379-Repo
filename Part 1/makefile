CC=gcc
CFLAGS=-Wall -Wextra -m32

all: pktgen router

pktgen: pktgen.c
	$(CC) $(CFLAGS) -o pktgen pktgen.c

router: router.c
	$(CC) $(CFLAGS) -o router router.c

clean:
	-rm -rf *.o pktgen router *.out stats*

tar:
	tar cvf 379djphan_a2p1.tar *.c makefile readme.txt
