CC=clang
CFLAGS=-g -Wall -std=c89 -pedantic
LDFLAGS=
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=../libchesslib.a
EXE=chess-shell

all: $(EXE)

$(EXE): $(OBJS) $(LIB)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -o $(EXE) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c
	$(CC) $(CFLAGS) -MM $< -MF $@

ifneq ($(MAKECMDGOALS), clean)
    -include $(SRCS:%.c=%.d)
endif

.PHONY: clean

clean:
	rm -f *.o *.d $(EXE)
