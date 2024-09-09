.PHONY: all clean

CC = gcc
CFLAGS += -std=c17
# CFLAGS += -O0 -g
CFLAGS += -g

all: sim8086

sim8086: sim8086.o
	$(CC) $(CFLAGS) sim8086.o -o sim8086

sim8086.o: sim8086.c sim8086.h
	$(CC) $(CFLAGS) -c sim8086.c -o sim8086.o
clean:
	rm -f *.o sim8086
	rm -f result.asm