
SOURCE = test.cpp literal.cpp benchmark.cpp 
HEADER = alloc.h include/base.h include/allocator.h include/pointer.h \
	include/list.h include/tree.h include/literal.h include/format.h include/win-utf.h
FLAGS = -std=c++98 -pedantic -fno-exceptions -Iinclude -Wall -Wno-parentheses

debug: $(SOURCE) $(HEADER)
	g++ -g -DDEBUG $(FLAGS) $(SOURCE) -o test

release: $(SOURCE) $(HEADER)
	g++ -O3 $(FLAGS) $(SOURCE) -o test
