CFLAGS = -Wall -g -std=c11
BIN = bin/
INC = include/
SRC = src/
XML_PATH = /usr/include/libxml2
LIB = include/

# Standard common makefile
parser: ../libsvgparse.so

#REMOVE BEFORE SUBMISSION
debug: test

test: $(SRC)tester.c $(BIN)libsvgparse.so
	gcc $(CFLAGS) -L./bin $(SRC)tester.c -I$(XML_PATH) -I$(INC) -o $(BIN)test -lsvgparse -lxml2

../libsvgparse.so: $(BIN)SVGParser.o $(BIN)LinkedListAPI.o
	gcc -shared -o ../libsvgparse.so $(BIN)SVGParser.o $(BIN)LinkedListAPI.o -lxml2 -lm

$(BIN)SVGParser.o: $(SRC)SVGParser.c $(INC)LinkedListAPI.h $(INC)SVGParser.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGParser.c -o $(BIN)SVGParser.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	gcc -c -fpic $(CFLAGS) -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

clean:
	rm $(BIN)*.o $(BIN)*.so 
