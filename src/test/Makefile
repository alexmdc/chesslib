CC=clang
CFLAGS=-g -Wall -std=c89 -pedantic -I/opt/local/include
LDFLAGS=-L.. -L/opt/local/lib -lchesslib -lcunit -lncurses
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=../libchesslib.a
EXE=chess-test

all: $(EXE)

$(EXE): $(OBJS) $(LIB)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXE) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o $(EXE)
