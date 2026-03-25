CC=gcc
TARGET=kv_store
SRCS=kv_store.c reactor.c ntyco.c io_uring.c  kvs_array.c kvs_rbtree.c kvs_hash.c
FLAGS=-I ./NtyCo/core/ -L ./NtyCo/ -luring -lntyco
all:
	$(CC) -o $(TARGET) $(SRCS) $(FLAGS)
	


clean: 
	rm -rf $(TARGET)
