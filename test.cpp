/***************************************************************************************************
* Nothrow Template Library : test.cpp : test suite
****************************************************************************************************

Copyright (c) 2012 Vabishchevich Nikolay aka }:+()___ [Smile]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***************************************************************************************************/

#include "alloc.h"
#include <cstdio>



MemoryHandler mem_handler;


void test_literal();
void test_format();
void test_win_utf();
void test_pointer();
void test_list();
void test_tree();


int main()
{
    std::setbuf(stdout, 0);

    test_literal();
    test_format();
    test_win_utf();
    test_pointer();
    test_list();
    test_tree();

    std::printf("\nTest completed successfully.\n");
    return 0;
}
