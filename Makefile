CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread

all: snake

snake: main.o klient.o server.o
	$(CC) -o snake main.o klient.o server.o $(LDFLAGS)

main.o: main.c zdielaneData.h
klient.o: klient.c zdielaneData.h
server.o: server.c zdielaneData.h

clean:
	rm -f *.o snake
