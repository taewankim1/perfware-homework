CXX       := clang++
OPTS      := -g -O0
LANG      := -std=c++17
PICKY     := -Wall -Wextra -Werror
INCLUDE   := -Iinclude
CXXFLAGS  := $(LANG) $(OPTS) $(PICKY) $(INCLUDE)

SRC_DIR   := src
BUILD_DIR := build


LIBS		:=
OS		:= $(shell uname -s)
ifeq ($(OS),Linux)
	LIBS	+= -lpthread
endif

all: haversine.exe read_haversine.exe

# Build object for haversine.cpp
$(BUILD_DIR)/haversine.o: $(SRC_DIR)/haversine.cpp | $(BUILD_DIR)
		$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/haversine.cpp -o $@

# Build object for read_haversine.cpp
$(BUILD_DIR)/read_haversine.o: $(SRC_DIR)/read_haversine.cpp | $(BUILD_DIR)
		$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/read_haversine.cpp -o $@

# Build executable for haversine
haversine.exe: $(BUILD_DIR)/haversine.o
		$(CXX) $(CXXFLAGS) $(BUILD_DIR)/haversine.o -o $@

# Build executable for read_haversine
read_haversine.exe: $(BUILD_DIR)/read_haversine.o
		$(CXX) $(CXXFLAGS) $(BUILD_DIR)/read_haversine.o -o $@


# Cleanup
clean:
		rm -f *~ $(BUILD_DIR)/*.o *.exe
		rm -rf *.exe.dSYM
		rm -f haversine.json

.PHONY: clean all