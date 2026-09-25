CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

smoke: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) smoke