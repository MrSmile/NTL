
all: base.h allocator.h pointer.h list.h tree.h literal.h test.cpp benchmark.cpp
	g++ -g -DDEBUG -pedantic -fno-exceptions test.cpp benchmark.cpp -o test
#	g++ -O3 -fno-exceptions test.cpp benchmark.cpp -o test
