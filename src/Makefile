CC=g++

CFLAGS=-c -Wall

LIBS=-lncurses -lpthread

all: wheel

wheel: printer.o menu.o main.o puzzles.o
	$(CC) printer.o puzzles.o menu.o main.o -o wheel ${LIBS}

main.o: main.c wof.h menu.h printer.h
	$(CC) $(CFLAGS) main.c

menu.o: menu.c wof.h menu.h printer.h
	$(CC) $(CFLAGS) menu.c

puzzles.o: puzzles.c puzzles.h
	$(CC) $(CFLAGS) puzzles.c

printer.o: printer.c wof.h menu.h printer.h puzzles.h
	$(CC) $(CFLAGS) printer.c

clean:
	rm -rf *~ *.o wheel

clean_o:
	rm -rf *~ *.o
