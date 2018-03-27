CC = gcc
CFLAGS = -g -Wall -pedantic -std=gnu99
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
LIBS = game.o messages.o -lm
PLAYER_LIBS = player_library.o

.PHONY: clean

all: $(OBJECTS)
	$(CC) $(LIBS) game_control.o 2310express.o -o 2310express
	$(CC) $(LIBS) $(PLAYER_LIBS) acrophobe.o -o acrophobe
	$(CC) $(LIBS) $(PLAYER_LIBS) bandit.o -o bandit
	$(CC) $(LIBS) $(PLAYER_LIBS) spoiler.o -o spoiler

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f 2310express acrophobe bandit spoiler *.o
