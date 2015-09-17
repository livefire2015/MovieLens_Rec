CXX = g++
CXXFLAGS = -O3 -Wall -std=c++11

main: main.cpp user.cpp user.hpp moviewatchers.cpp moviewatchers.hpp timer.c timer.h
	$(CXX) $(CXXFLAGS) -o main main.cpp user.cpp moviewatchers.cpp timer.c

clean:
	rm -f main *~
