/***************************************************************************************************
* Nothrow Template Library : benchmark.cpp : test suite
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

#include "fmt-int.h"
#include "tree.h"
#include <cstdio>
#include <ctime>
#include <map>

using namespace std;
using namespace NTL;



const double c2s = 1.0 / CLOCKS_PER_SEC;



void time_string()
{
    const int N = 10000000;

    printf("String benchmark: ");  fflush(stdout);

    clock_t tm0 = clock();

    srand(12345);
    size_t len1 = 0;
    for(int i = 0; i < N; i++)
    {
        String str = format("x = %1; y = %2") % dec(rand()) % dec(rand());
        len1 += str.length();
    }

    clock_t tm1 = clock();

    srand(12345);
    size_t len2 = 0;
    static char buf[256];
    for(int i = 0; i < N; i++)
    {
        len2 += sprintf(buf, "x = %d; y = %d", rand(), rand());
    }

    clock_t tm2 = clock();

    if(len1 != len2)debug_break();

    printf("NTL::format ~ %.3fs; sprintf ~ %.3fs;\n", (tm1 - tm0) * c2s, (tm2 - tm1) * c2s);
}



struct Data
{
    int num;
};

struct TestNode : public TreeNode<TestNode>, public SimpleKey<int>
{
    Data data;

    TestNode(int n) : SimpleKey<int>(n)
    {
    }
};

void time_tree()
{
    const int N = 1000000;

    printf("Tree benchmark: ");  fflush(stdout);

    clock_t tm0 = clock();

    srand(12345);
    OwningTree<TestNode> tree1;
    for(int i = 0; i < N; i++)
    {
        tree1.get(rand());  tree1.get(rand());
        delete tree1.find_place(rand()).after();
    }

    clock_t tm1 = clock();

    srand(12345);
    map<int, Data> tree2;
    for(int i = 0; i < N; i++)
    {
        tree2[rand()] = Data();  tree2[rand()] = Data();
        map<int, Data>::iterator ptr = tree2.lower_bound(rand());
        if(ptr != tree2.end())tree2.erase(ptr);
    }

    clock_t tm2 = clock();

    tree1.check_();  map<int, Data>::iterator ptr = tree2.begin();
    for(TestNode *node = tree1.first(); node; node = node->next(), ++ptr)
        if(ptr == tree2.end() || ptr->first != node->id())debug_break();
    if(ptr != tree2.end())debug_break();

    printf("OwningTree<TestNode> ~ %.3fs; map<int, Data> ~ %.3fs;\n", (tm1 - tm0) * c2s, (tm2 - tm1) * c2s);
}


int main()
{
    time_string();
    time_tree();
    return 0;
}
