
SOURCE = test.cpp benchmark.cpp 
HEADER = include/base.h include/allocator.h include/pointer.h \
	include/list.h include/tree.h include/literal.h
FLAGS = -std=c++98 -pedantic -fno-exceptions -Iinclude

debug: $(SOURCE) $(HEADER)
	g++ -g -DDEBUG $(FLAGS) $(SOURCE) -o test

release: $(SOURCE) $(HEADER)
	g++ -O3 $(FLAGS) $(SOURCE) -o test
