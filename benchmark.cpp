#include <map>
#include <iostream>
#include "tree.h"

using namespace std;


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

    time_t tm0 = time(0);

    srand(12345);
    OwningTree<TestNode> tree1;
    for(int i = 0; i < N; i++)
    {
        tree1.get(rand());  tree1.get(rand());
        delete tree1.find_next(rand());
    }

    time_t tm1 = time(0);

    srand(12345);
    map<int, Data> tree2;
    for(int i = 0; i < N; i++)
    {
        tree2[rand()] = Data();  tree2[rand()] = Data();
        map<int, Data>::iterator ptr = tree2.lower_bound(rand());
        if(ptr != tree2.end())tree2.erase(ptr);
    }

    time_t tm2 = time(0);

    tree1.check_();  map<int, Data>::iterator ptr = tree2.begin();
    for(TestNode *node = tree1.first(); node; node = node->next(), ++ptr)
        if(ptr == tree2.end() || ptr->first != node->id())debug_break();
    if(ptr != tree2.end())debug_break();

    cout << "Tree benchmark: OwningTree<TestNode> = " << tm1 - tm0 << "; map<int, Data> = " << tm2 - tm1 << ";\n";
}
