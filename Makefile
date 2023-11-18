CC = gcc
CFLAGS = -Wall -pthread

all: mmparallel

mmparallel: mmparallel.c
	$(CC) $(CFLAGS) -o mmparallel mmparallel.c

clean:
	rm -f mmparallel
