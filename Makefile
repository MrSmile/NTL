
all: base.h allocator.h pointer.h list.h tree.h literal.h test.cpp
	g++ -g -DDEBUG -pedantic -fno-exceptions test.cpp -o test
