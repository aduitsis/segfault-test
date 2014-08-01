CC=gcc
CFLAGS= -Wall -std=gnu99
SOURCES=segfault-test.c

segfault-test: $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@  
