.PHONY: test clean

test: test.out
	./test.out

test.out: test.o
	$(CXX) -o test.out test.o

test.o: test.cpp ../src/lolita.hpp
	$(CXX) -c test.cpp -std=c++11 -I../src -W -Wall -Wextra -O3

clean:
	$(RM) test.o test.out