CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I./include
LDFLAGS = -pthread

SRC = src/main.cpp src/game.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = tetris

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
