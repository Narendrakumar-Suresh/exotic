CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

SRCS = src/main.cpp src/lexer/lexer.cpp src/parser/parser.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = exc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run