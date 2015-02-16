CC=clang
CFLAGS=-g -Wall -std=c89 -pedantic
LDFLAGS=-L.. -lchesslib
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
EXE=chess-shell

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXE) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o $(EXE)
