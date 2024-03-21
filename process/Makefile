CC=gcc
CFLAGS=-Wall -g -lpthread
SRCS=$(wildcard *.c)
PROGS=$(patsubst %.c,%,$(SRCS))

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(PROGS)