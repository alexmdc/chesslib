CC=gcc
CFLAGS=-g -Wall -c
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=libchesslib.a

all: $(LIB)

$(LIB): $(OBJS)
	libtool -static -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(LIB)
