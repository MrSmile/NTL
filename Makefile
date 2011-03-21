
all: base.h literal.h test.cpp
	g++ -pedantic -fno-exceptions -O2 test.cpp -o test
