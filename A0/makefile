CC = gcc
CFLAGS = -std=c99 -Wall -I$(IDIR)

IDIR = ./include/
SRCDIR = ./src/
BINDIR = ./bin/

SOURCES = $(SRCDIR)*.c 

all: liblist

liblist: $(BINDIR)liblist.so

$(BINDIR)liblist.so: $(BINDIR)liblist.o
	$(CC) -shared $(BINDIR)liblist.o -o $@

$(BINDIR)liblist.o: $(SRCDIR)LinkedListAPI.c $(IDIR)LinkedListAPI.h
	$(CC) $(CFLAGS) -c $(SRCDIR)LinkedListAPI.c -o $@

run:
	$(BINDIR)LinkedListAPI 

clean:
	rm ./bin/*.o ./LinkedListAPI