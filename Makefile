CC = gcc
CFLAGS = -g -W -Wall -pedantic
LDFLAGS =
OBJ = server.o Own_lib.o common.o


all: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o server
	
server.o : server.c functions.h
Own_lib.o : Own_lib.c functions.h
common.o : common.c functions.h


clean:
	rm -f $(OBJ)
	
clean_all:
	rm -f $(OBJ) server
