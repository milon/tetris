CXX = g++
CXXFLAGS = -std=c++11 -Wall -g $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

TARGET = tetris
SRCS = tetris.cpp graphics_compat.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
