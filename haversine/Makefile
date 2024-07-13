CXX       := clang++

OPTS      := -g -O0
LANG      := -std=c++14
PICKY     := -Wall

CXXFLAGS  := $(LANG) $(OPTS) $(PICKY)

.PHONY	  : clean all

all		  : haversine.exe

LIBS		:=
OS		:= $(shell uname -s)
ifeq ($(OS),Linux)
	LIBS	+= -lpthread
endif


haversine.o 	: haversine.cpp 
		  $(CXX) $(CXXFLAGS) -c haversine.cpp -o haversine.o

haversine.exe	: haversine.cpp
		  $(CXX) $(CXXFLAGS) haversine.cpp -o haversine.exe


clean		:
			/bin/rm -f *~ haversine.o haversine.exe
			/bin/rm -rf *.exe.dSYM