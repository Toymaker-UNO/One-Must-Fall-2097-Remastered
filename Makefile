CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = log_example
SOURCES = log.cpp log_example.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.log

.PHONY: all clean
