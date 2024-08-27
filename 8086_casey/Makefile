# Compiler
CC = clang++

# Compiler flags
CFLAGS = -std=c++11 -Iinclude -Wall

# Source files
SRCS = src/sim86.cpp src/sim86_memory.cpp src/sim86_decode.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = build/sim86

# Default target
all: $(EXEC)

# Link the executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compile source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(EXEC)

# Phony targets
.PHONY: all clean