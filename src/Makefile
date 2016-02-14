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

%.d: %.c
	$(CC) $(CFLAGS) -MM $< -MF $@

ifneq ($(MAKECMDGOALS), clean)
    -include $(SRCS:%.c=%.d)
endif

clean: $(SUBDIRS)
	rm -f *.o *.d $(LIB)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKEOPTS) $(MAKECMDGOALS)

.PHONY: clean $(SUBDIRS)
