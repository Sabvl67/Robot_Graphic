# Makefile for OpenGL Graphics Project (MacOS & Linux)

# Compilers
CXX = g++
CC = gcc

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude
CFLAGS = -Wall -Wextra -Iinclude

# Detect OS
UNAME_S := $(shell uname -s)

# Platform-specific settings
ifeq ($(UNAME_S),Linux)
    # Linux settings
    LDFLAGS = -lGL -lglfw -ldl -lpthread
    TARGET = graphics_program
endif

ifeq ($(UNAME_S),Darwin)
    # macOS settings
    BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo /usr/local)
    CXXFLAGS += -I$(BREW_PREFIX)/include
    CFLAGS += -I$(BREW_PREFIX)/include
    LDFLAGS = -L$(BREW_PREFIX)/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    TARGET = graphics_program
endif

# Source files
SOURCES = src/main.cpp src/glad.c src/shader.cpp src/cube.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)

# Default target
all: $(TARGET)

# Link the program
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@echo "Build successful! Run with: ./$(TARGET)"

# Compile C++ source files
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C source files
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Rebuild
rebuild: clean all

.PHONY: all clean run rebuild
