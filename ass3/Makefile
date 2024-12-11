# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++2a -pthread -g

# Target executable name
TARGET = ex3.out

# Source files
SRCS = main.cpp BoundedBuffer.cpp Producer.cpp UnboundedBuffer.cpp Dispatcher.cpp CoEditor.cpp ScreenManager.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Dependencies (header files)
DEPS = BoundedBuffer.h Producer.h UnboundedBuffer.h Dispatcher.h CoEditor.h ScreenManager.h

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
