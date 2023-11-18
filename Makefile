CC = gcc
CFLAGS_MMPARALLEL = -Wall -pthread
CFLAGS_MATMULSEQ_FILE = -Wall

all: coarsegrain_mmparallel finegrain_mmparallel matmulseq_file

coarsegrain_mmparallel: coarsegrain_mmparallel.c
	$(CC) $(CFLAGS_MMPARALLEL) -o coarsegrain_mmparallel coarsegrain_mmparallel.c

finegrain_mmparallel: finegrain_mmparallel.c
	$(CC) $(CFLAGS_MMPARALLEL) -o finegrain_mmparallel finegrain_mmparallel.c

matmulseq_file: matmulseq_file.c
	$(CC) $(CFLAGS_MATMULSEQ_FILE) -o matmulseq_file matmulseq_file.c

clean:
	rm -f coarsegrain_mmparallel finegrain_mmparallel matmulseq_file
