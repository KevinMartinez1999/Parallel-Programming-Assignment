CC = gcc
CFLAGS_MMPARALLEL = -Wall -pthread
CFLAGS_MATMULSEQ_FILE = -Wall

all: matmul_seq matmul_coarsegrain matmul_finegrain

matmul_seq: matmul_seq.c
	$(CC) $(CFLAGS_MATMULSEQ_FILE) -o matmul_seq matmul_seq.c

matmul_coarsegrain: matmul_coarsegrain.c
	$(CC) $(CFLAGS_MMPARALLEL) -o matmul_coarsegrain matmul_coarsegrain.c

matmul_finegrain: matmul_finegrain.c
	$(CC) $(CFLAGS_MMPARALLEL) -o matmul_finegrain matmul_finegrain.c

clean:
	rm -f matmul_seq matmul_coarsegrain matmul_finegrain
