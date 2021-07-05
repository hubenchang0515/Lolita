.PHONY: test profile memcheck clean

test: test.out
	./test.out

memcheck: test.out
	valgrind --tool=memcheck ./test.out

profile: gmon.out
	gprof ./pg_test.out gmon.out

gmon.out: pg_test.out
	./pg_test.out

test.out: test.o
	$(CXX) -o test.out test.o

test.o: test.cpp ../src/lolita.hpp
	$(CXX) -c test.cpp -std=c++11 -I../src -W -Wall -Wextra -O3

pg_test.out: pg_test.o
	$(CXX) -o pg_test.out pg_test.o -pg

pg_test.o: test.cpp ../src/lolita.hpp
	$(CXX) -o pg_test.o -c test.cpp -std=c++11 -I../src -W -Wall -Wextra -O3 -pg

clean:
	$(RM) *.o *.out