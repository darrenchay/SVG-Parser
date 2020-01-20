<<<<<<< HEAD
UNAME := $(shell uname)
CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS = -L.
INCDIR = ./include
SRCDIR = ./src
BINDIR = ./bin
=======
CFLAGS = -Wall -g -std=c11
BIN = bin/
INC = include/
SRC = src/
XML_PATH = /usr/include/libxml2
>>>>>>> 1d2bfb58fab744eab2fe07c1928372f16187e0be

# Standard common makefile
parser: $(BIN)libsvgparse.so

debug: $(BIN)main_debug

<<<<<<< HEAD
all: $(BINDIR)StructListDemo
	
$(BINDIR)StructListDemo: $(BINDIR)StructListDemo.o $(BINDIR)liblist.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o StructListDemo StructListDemo.o  -llist
	
StructListDemo.o: StructListDemo.c
	$(CC) $(CFLAGS) -c StructListDemo.c 
	
liblist.so: LinkedListAPI.o
	$(CC) -shared -o liblist.so LinkedListAPI.o

LinkedListAPI.o: LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic LinkedListAPI.c
=======
$(BIN)main_debug: $(SRC)main_tester_function.c $(BIN)libsvgparse.so
	gcc -c $(CFLAGS) -I$(INC) $(SRC)main_tester_function.c -o $@ -lxml2 -lm -llibsvgparse

$(BIN)libsvgparse.so: $(BIN)SVGParser.o $(BIN)LinkedListAPI.o
	gcc -shared -o $(BIN)libsvgparse.so $(BIN)SVGParser.o $(BIN)LinkedListAPI.o -lxml2 -lm
>>>>>>> 1d2bfb58fab744eab2fe07c1928372f16187e0be

$(BIN)SVGParser.o: $(SRC)SVGParser.c $(INC)LinkedListAPI.h $(INC)SVGParser.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGParser.c -o $(BIN)SVGParser.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	gcc -c -fpic $(CFLAGS) -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

clean:
	rm $(BIN)*.o $(BIN)*.so
