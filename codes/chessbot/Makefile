# Simple Makefile for Lumin Chess Engine

CXX = g++
CXXFLAGS = -std=c++17 -O3
TARGET = lumin
SOURCES = Lumin.cpp movegen.cpp magic.cpp nonmagic.cpp attacks.cpp bitboard.cpp position.cpp movedef.cpp perftest.cpp

# Build the program
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

# Clean build files
clean:
	rm -f $(TARGET)

# Run the program
run:$(TARGET)
	./$(TARGET)

# Rebuild everything
rebuild: clean $(TARGET)

.PHONY: clean run rebuild
