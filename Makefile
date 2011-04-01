
SOURCE = test.cpp benchmark.cpp 
HEADER = base.h allocator.h pointer.h list.h tree.h literal.h

all: $(SOURCE) $(HEADER)
	g++ -O3 -fno-exceptions $(SOURCE) -o test

debug: $(SOURCE) $(HEADER)
	g++ -g -DDEBUG -pedantic -fno-exceptions $(SOURCE) -o test
