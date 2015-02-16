CC=clang
CFLAGS=-g -Wall -std=c89 -pedantic
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
SUBDIRS=test shell
LIB=libchesslib.a

all: $(LIB) $(SUBDIRS)

$(LIB): $(OBJS)
	libtool -static -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: $(SUBDIRS)
	rm -f *.o $(LIB)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKEOPTS) $(MAKECMDGOALS)

.PHONY: clean $(SUBDIRS)
