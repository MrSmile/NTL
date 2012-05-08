
SOURCE = test.cpp literal.cpp format.cpp win-utf.cpp pointer.cpp list.cpp tree.cpp
HEADER = alloc.h include/base.h include/allocator.h include/pointer.h \
	include/list.h include/tree.h include/literal.h include/win-utf.h \
	include/format.h include/fmt-int.h
FLAGS = -std=c++98 -pedantic -fno-exceptions -Iinclude -Wall -Wno-parentheses -Wno-long-long


all: test bench

test: $(SOURCE) $(HEADER)
	g++ -g -O0 -DDEBUG $(FLAGS) $(SOURCE) -o test

bench: benchmark.cpp $(HEADER)
	g++ -O3 -fwhole-program -mtune=native $(FLAGS) benchmark.cpp -o bench
