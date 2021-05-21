.PHONY: all clean

all: test.out
	./test.out

test.out: test.o
	g++ -o test.out test.o

test.o: test.cpp ../src/lolita.hpp
	g++ -c test.cpp -std=c++11 -I../src -W -Wall -Wextra -O3


clean:
	$(RM) test.o test.out