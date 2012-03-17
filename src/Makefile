CC=gcc
CFLAGS=-g -Wall -c
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
SUBDIRS=test shell
LIB=libchesslib.a

all: $(LIB) $(SUBDIRS)

$(LIB): $(OBJS)
	libtool -static -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean: $(SUBDIRS)
	rm -f *.o $(LIB)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKEOPTS) $(MAKECMDGOALS)

.PHONY: clean $(SUBDIRS)
