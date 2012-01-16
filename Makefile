
SOURCE = test.cpp benchmark.cpp 
HEADER = base.h allocator.h pointer.h list.h tree.h literal.h

debug: $(SOURCE) $(HEADER)
	g++ -g -DDEBUG -std=c++98 -pedantic -fno-exceptions $(SOURCE) -o test

release: $(SOURCE) $(HEADER)
	g++ -O3 -fno-exceptions $(SOURCE) -o test
