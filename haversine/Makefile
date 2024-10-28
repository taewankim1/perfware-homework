CXX       := clang++

OPTS      := -g -O0
LANG      := -std=c++17
PICKY     := -Wall -Wextra -Werror
INCLUDE   := -Iinclude

CXXFLAGS  := $(LANG) $(OPTS) $(PICKY) $(INCLUDE)

.PHONY	  : clean all

all		  : haversine.exe

LIBS		:=
OS		:= $(shell uname -s)
ifeq ($(OS),Linux)
	LIBS	+= -lpthread
endif


build/haversine.o 	: src/haversine.cpp 
		  $(CXX) $(CXXFLAGS) -c src/haversine.cpp -o build/haversine.o

haversine.exe	: build/haversine.o
		  $(CXX) $(CXXFLAGS) build/haversine.o -o haversine.exe


clean		:
			rm -f *~ build/haversine.o haversine.exe
			rm -rf *.exe.dSYM
			rm -f haversine.json