CC=gcc
CFLAGS=-pthread -Wall

all: producer_consumer problem2

producer_consumer: producer_consumer.c peterson.c thread_wrapper.c concurrent_queue.c
	$(CC) $(CFLAGS) producer_consumer.c peterson.c thread_wrapper.c concurrent_queue.c -o producer_consumer

problem2: problem2.c peterson.c thread_wrapper.c
	$(CC) $(CFLAGS) problem2.c peterson.c thread_wrapper.c -o problem2

clean:
	rm -f producer_consumer problem2
