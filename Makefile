
SOURCE = test.cpp literal.cpp format.cpp win-utf.cpp pointer.cpp list.cpp tree.cpp
HEADER = alloc.h include/ntl/base.h include/ntl/allocator.h include/ntl/pointer.h \
	include/ntl/list.h include/ntl/tree.h include/ntl/literal.h include/ntl/win-utf.h \
	include/ntl/format.h include/ntl/fmt-int.h
FLAGS = -std=c++98 -pedantic -fno-exceptions -Iinclude/ntl -Wall -Wno-parentheses -Wno-long-long


all: test bench

test: $(SOURCE) $(HEADER)
	g++ -g -O0 -DDEBUG $(FLAGS) $(SOURCE) -o test

bench: benchmark.cpp $(HEADER)
	g++ -Ofast -fwhole-program -mtune=native $(FLAGS) benchmark.cpp -o bench

clean:
	rm test bench
