
all: base.h literal.h test.cpp
	g++ -pedantic -fno-exceptions -g test.cpp -o test
