CC = gcc
CFLAGS_MMPARALLEL = -Wall -pthread
CFLAGS_MATMULSEQ_FILE = -Wall

all: mmparallel matmulseq_file

mmparallel: mmparallel.c
	$(CC) $(CFLAGS_MMPARALLEL) -o mmparallel mmparallel.c

matmulseq_file: matmulseq_file.c
	$(CC) $(CFLAGS_MATMULSEQ_FILE) -o matmulseq_file matmulseq_file.c

clean:
	rm -f mmparallel matmulseq_file
