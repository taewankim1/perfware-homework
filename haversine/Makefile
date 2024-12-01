CXX       := clang++
OPTS      := -g -O0
LANG      := -std=c++17
PICKY     := -Wall -Wextra
INCLUDE   := -Iinclude
CXXFLAGS  := $(LANG) $(OPTS) $(PICKY) $(INCLUDE)

SRC_DIR   := src
BUILD_DIR := build

LIBS		:=
OS		:= $(shell uname -s)
ifeq ($(OS),Linux)
	LIBS	+= -lpthread
endif

# List of executables to build
TARGETS := haversine_generator haversine_read_simple listing_0071_os_timer_main listing_0072_cpu_timer_main listing_0073_cpu_timer_guessfreq_main listing_0075_timed_haversine_main
EXES := $(addsuffix .exe, $(TARGETS))
OBJS := $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(TARGETS)))

# Pattern rule for object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for executables
%.exe: $(BUILD_DIR)/%.o
	$(CXX) $(CXXFLAGS) $< -o $@

# # Create build directory
# $(BUILD_DIR):
# 	mkdir -p $(BUILD_DIR)

# All target
all: $(EXES) $(OBJS)

# Cleanup
clean:
	rm -f *~ $(BUILD_DIR)/*.o *.exe
	rm -rf *.exe.dSYM
	rm -f haversine.json

.PHONY: clean all