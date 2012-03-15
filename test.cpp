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
#include "pointer.h"
#include "tree.h"
#include "list.h"
#include <iostream>
#include <cstdio>



struct Node : public NTL::SimpleKey<int>
{
    static int count;

    Node(int n = 777) : SimpleKey<int>(n)
    {
        std::cout << "Created " << id() << ", total " << ++count << std::endl;
    }

    Node(const Node &node) : SimpleKey<int>(node.id() + 1000)
    {
        std::cout << "Copied " << node.id() << " -> " << id() << ", total " << ++count << std::endl;
    }

    ~Node()
    {
        std::cout << "Deleted " << id() << ", left " << --count << std::endl;
    }

    void *operator new(size_t size, const std::nothrow_t &) throw()
    {
        const int max = 100;
        if(count >= max)return 0;  return new_nt char[size];
    }
};

int Node::count = 0;

/*void test_pointer()
{
    std::cout << "\n=== Testing Pointers ===\n";
    {
        NTL::Pointer<Node> del(new_nt Node(555));
        NTL::Pointer<Node> node(new_nt Node(666));
        NTL::Array<Node> array(10);  del = new_nt Node(888);
        delete node.detach();
    }
    std::cout << std::endl;
}*/


struct NodeT1 : public NTL::TreeNode<NodeT1>, public Node
{
    NodeT1(int n) : Node(n)
    {
    }

    NodeT1(const NodeT1 &node) : Node(node)
    {
    }
};

struct NodeT2 : public NTL::IndexerNode<NodeT2>, public Node
{
    NodeT2(int n) : Node(n)
    {
    }

    NodeT2(const NodeT2 &node) : Node(node)
    {
    }
};

void test_tree()
{
    std::cout << "\n=== Testing Trees, Stage 1 ===\n";
    {
        NTL::OwningTree<NodeT1> tree;
        for(int i = 0; i < 10; i++)tree.get(i);
        tree.check_();
        {
            NTL::OwningTree<NodeT1> copy;
            if(!copy.copy(tree))std::cout << "Copy failed!\n";
            else delete copy.find(1005)->next();
            copy.check_();  swap(tree, copy);  copy.check_();
        }
        tree.check_();
        for(NodeT1 *node = tree.first(); node; node = node->next())
            std::cout << node->id() << ' ';  std::cout << std::endl;
        for(NodeT1 *node = tree.last(); node; node = node->prev())
            std::cout << node->id() << ' ';  std::cout << std::endl;
    }
    std::cout << "\n=== Testing Trees, Stage 2 ===\n";
    {
        NTL::OwningIndexer<NodeT2> tree;
        for(int i = 0; i < 10; i++)tree.get(i);
        {
            NTL::OwningIndexer<NodeT2> copy;  copy.check_();
            if(!copy.copy(tree))std::cout << "Copy failed!\n";
            else delete copy.find(1005)->next();
            copy.check_();  swap(tree, copy);  copy.check_();
        }
        std::cout << "Index of 1006: " << tree.find_place(1006).index() << std::endl;
        tree.insert(new_nt NodeT2(444), 4);  tree.check_();  size_t index = 0;
        for(NodeT2 *node = tree.first(); node; node = node->next(), index++)
            assert(tree[index] == node && node->index() == index);
        for(NodeT2 *node = tree.first(); node; node = node->next())
            std::cout << node->id() << ' ';  std::cout << std::endl;
        for(NodeT2 *node = tree.last(); node; node = node->prev())
            std::cout << node->id() << ' ';  std::cout << std::endl;
    }
    std::cout << std::endl;
}


struct NodeL1 : public NTL::StackNode<NodeL1>, public Node
{
    NodeL1(int n) : Node(n)
    {
    }

    NodeL1(const NodeL1 &node) : Node(node)
    {
    }
};

struct NodeL2 : public NTL::SimpleListNode<NodeL2>, public Node
{
    NodeL2(int n) : Node(n)
    {
    }

    NodeL2(const NodeL2 &node) : Node(node)
    {
    }
};

struct NodeL3 : public NTL::ListNode<NodeL3>, public Node
{
    NodeL3(int n) : Node(n)
    {
    }

    NodeL3(const NodeL3 &node) : Node(node)
    {
    }
};

/*void test_list()
{
    std::cout << "\n=== Testing Lists, Stage 1 ===\n";
    {
        NTL::OwningStack<NodeL1> list;
        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL1(i));
        list.first()->next()->next()->insert_next(new_nt NodeL1(777));
        delete list.first()->next()->take_next();
        NTL::OwningStack<NodeL1> move, copy;
        if(!copy.copy(list))std::cout << "Copy failed!\n";
        for(;;)
        {
            NodeL1 *node = list.take_first();  if(!node)break;
            std::cout << node->id() << ' ';  move.prepend(node);
        }
        std::cout << std::endl;  swap(list, move);
        for(NodeL1 *node = list.first(); node; node = node->next())
            std::cout << node->id() << ' ';  std::cout << std::endl;
    }
    std::cout << "\n=== Testing Lists, Stage 2 ===\n";
    {
        NTL::OwningSimpleList<NodeL2> list;
        for(int i = 0; i < 10; i++)list.append(new_nt NodeL2(i));
        list.insert(new_nt NodeL2(777), list.first()->next()->next());
        delete list.take(list.first());  list.prepend(new_nt NodeL2(666));
        NTL::OwningSimpleList<NodeL2> move, copy;
        if(!copy.copy(list))std::cout << "Copy failed!\n";
        for(;;)
        {
            NodeL2 *node = list.take_first();  if(!node)break;
            std::cout << node->id() << ' ';  move.prepend(node);
        }
        std::cout << std::endl;  swap(list, move);
        if(!list.prepend_copy(copy))std::cout << "Merge copy failed!\n";
        list.append(copy);
        for(NodeL2 *node = list.first(); node; node = node->next())
            std::cout << node->id() << ' ';  std::cout << std::endl;
    }
    std::cout << "\n=== Testing Lists, Stage 3 ===\n";
    {
        NTL::OwningList<NodeL3> list;
        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL3(i));
        list.first()->next()->next()->insert_next(new_nt NodeL3(777));
        list.first()->next()->next()->insert_prev(new_nt NodeL3(666));
        delete list.first()->next();
        NTL::OwningList<NodeL3> move, copy;
        if(!copy.copy(list))std::cout << "Copy failed!\n";
        for(;;)
        {
            NodeL3 *node = list.take_first();  if(!node)break;
            std::cout << node->id() << ' ';  move.prepend(node);
        }
        std::cout << std::endl;  swap(list, move);
        for(NodeL3 *node = list.first(); node; node = node->next())
            std::cout << node->id() << ' ';  std::cout << std::endl;
    }
    std::cout << std::endl;
}*/



MemoryHandler mem_handler;

void test_literal();
void test_format();
void test_win_utf();
void test_pointer();
void test_list();

void time_tree();

int main(int n, char **arg)
{
    std::setbuf(stdout, 0);
    mem_handler.make_fail(1);

    test_literal();
    test_format();
    test_win_utf();
    test_pointer();
    test_list();

    return 0;

    /*for(int i = 1; i < n; i++)
        if(!strcmp(arg[i], "string"))test_strings();
        else if(arg[i] == NTL::Literal("pointer"))test_pointer();
        else if(arg[i] == NTL::Literal("tree"))
        {
            test_tree();  time_tree();
        }
        else if(arg[i] == NTL::Literal("list"))test_list();

    if(n <= 1)std::cout << "Usage: test [string] [pointer] [tree] [list]\n";
    return 0;*/
}
