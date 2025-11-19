# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -O2 -fopenmp

# Source files
CPP_SOURCES = $(wildcard *.cpp)
C_SOURCES = glad/glad.c

# Output executable
TARGET = inOneWeekend.exe

# Libraries
LIBS = -lglfw3 -lopengl32 -lgdi32

# Default rule
all: $(TARGET)

# Build rule
$(TARGET): $(CPP_SOURCES) $(C_SOURCES)
	$(CXX) $(CXXFLAGS) $(CPP_SOURCES) $(C_SOURCES) -o $(TARGET) $(LIBS)

# Run rule
run: $(TARGET)
	./$(TARGET)

# Clean up
clean:
	rm -f $(TARGET)
