
HASHMAP=hashmap

SOURCES=hashmap.c testHash.c

CC=gcc
CFLAGS= -Wall -pedantic -std=c99 -ggdb

all: $(HASHMAP)

$(HASHMAP): $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(HASHMAP)

.PHONY: all, clean