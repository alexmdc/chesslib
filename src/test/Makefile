CC=gcc
CFLAGS=-g -Wall -ansi -pedantic -I/opt/local/include
LDFLAGS=-L.. -L/opt/local/lib -lchesslib -lcunit -lncurses
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
EXE=chess-test

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXE) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm *.o $(EXE)
