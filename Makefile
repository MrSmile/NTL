
all: base.h allocator.h pointer.h tree.h literal.h test.cpp
	g++ -pedantic -fno-exceptions -g test.cpp -o test
