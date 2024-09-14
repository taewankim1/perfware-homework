.PHONY: all clean

CC = gcc

CFLAGS += -std=c17 -Iinclude -Wall # -O0
CFLAGS += -g

# Source files
SRCS = src/sim8086.c

# Object files (place them in the build folder)
OBJS = $(SRCS:src/%.c=build/%.o)

# Executable name
EXEC = sim8086

all: $(EXEC)

# Compile source files into object files
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
# $(OBJS) : $(SRCS)

# Link the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# sim8086: build/sim8086.o
# 	$(CC) $(CFLAGS) build/sim8086.o -o sim8086

# build/sim8086.o: src/sim8086.c # sim8086.h
# 	$(CC) $(CFLAGS) -c src/sim8086.c -o build/sim8086.o

clean:
	rm -f build/*.o $(EXEC)