/***************************************************************************************************
* Nothrow Template Library : list.cpp : test suite for list.h
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
#include "list.h"
#include <cstdio>



template<template <typename> class C, typename T> void check_sort(int n, int m, int k)
{
    mem_handler.reset();
    {
        C<T> list;
        for(int i = 0; i < n; i++)
        {
            list.prepend(new_nt T((i * m + k) % n));
        }
        mem_handler.check(n, 0);

        list.sort();
        mem_handler.check(0, 0);

        T *node = list.first();
        for(int i = 0; i < n; i++)
        {
            assert(node->num == i);  node = node->next();
        }
        assert(!node);
        mem_handler.check(0, 0);
    }
    mem_handler.check(0, n);
}


struct NodeL1 : public NTL::StackNode<NodeL1>
{
    int num;

    NodeL1(int n) : num(n)
    {
    }

    NodeL1(const NodeL1 &node) : num(node.num + 100)
    {
    }

    int cmp(const NodeL1 &node) const
    {
        return num - node.num;
    }
};

void test_stack_container()
{
    std::printf("\nTesting Stack container:\n  constructors... ");

    mem_handler.reset();
    {
        NTL::EmptyAllocator<NodeL1> alloc;  NTL::Stack<NodeL1> list(alloc);
        assert(list.empty() && !list && list == false);
        NodeL1 *node = list.first();
        const NodeL1 *cnode = static_cast<const NTL::Stack<NodeL1> &>(list).first();
        assert(!node && cnode == node);
        mem_handler.check(0, 0);

        NTL::OwningStack<NodeL1> copy;
        node = copy.first();
        cnode = static_cast<const NTL::OwningStack<NodeL1> &>(copy).first();
        assert(!node && !cnode);
        mem_handler.check(0, 0);

        std::printf("OK\n  prepend(), take_first()... ");

        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL1(9 - i));
        assert(!list.empty() && !!list && list == true);
        mem_handler.check(10, 0);

        node = list.first();
        for(int i = 0; i < 10; i++)
        {
            assert(node->num == i);
            cnode = static_cast<const NodeL1 *>(node)->next();
            node = node->next();
            assert(cnode == node);
        }
        assert(!node);
        mem_handler.check(0, 0);

        for(int i = 0; i < 10; i++)
        {
            node = list.take_first();
            assert(node->num == i);
            delete node;
        }
        node = list.take_first();
        assert(!node);
        mem_handler.check(0, 10);

        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL1(9 - i));
        mem_handler.check(10, 0);

        std::printf("OK\n  copy(), swap()... ");

        mem_handler.make_fail(10);
        bool res = copy.copy(list);
        assert(!res && !copy.first());
        mem_handler.make_reliable();
        mem_handler.check(9, 9);

        res = copy.copy(list);
        assert(res && copy.first());
        mem_handler.check(10, 0);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        NTL::OwningStack<NodeL1> copy1;
        res = copy1.copy(copy);
        assert(res && copy1.first());
        mem_handler.check(10, 0);

        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        mem_handler.check(0, 0);

        swap(copy, copy1);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  insert_next(), take_next()... ");

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
        {
            NodeL1 *next = node->take_next();
            if(i < 9)
            {
                assert(next->num == i + 1);
                node->insert_next(next);
            }
            else assert(!next);
        }
        assert(!node);
        mem_handler.check(0, 0);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  clear(), destructors... ");

        copy1.clear();
        assert(copy1.empty());
        mem_handler.check(0, 10);

        while((node = list.take_first()))delete node;
        mem_handler.check(0, 10);
    }
    mem_handler.check(0, 10);

    std::printf("OK\n  sort()... ");

    check_sort<NTL::OwningStack, NodeL1>(0, 0, 0);
    check_sort<NTL::OwningStack, NodeL1>(1, 0, 0);
    check_sort<NTL::OwningStack, NodeL1>(2, 1, 0);
    check_sort<NTL::OwningStack, NodeL1>(2, 1, 1);
    check_sort<NTL::OwningStack, NodeL1>(3, 1, 0);
    check_sort<NTL::OwningStack, NodeL1>(3, 1, 1);
    check_sort<NTL::OwningStack, NodeL1>(3, 1, 2);
    check_sort<NTL::OwningStack, NodeL1>(3, 2, 2);
    check_sort<NTL::OwningStack, NodeL1>(3, 2, 1);
    check_sort<NTL::OwningStack, NodeL1>(3, 2, 0);
    check_sort<NTL::OwningStack, NodeL1>(4, 3, 3);
    check_sort<NTL::OwningStack, NodeL1>(27, 8, 11);
    check_sort<NTL::OwningStack, NodeL1>(64, 17, 47);
    check_sort<NTL::OwningStack, NodeL1>(65, 48, 20);
    check_sort<NTL::OwningStack, NodeL1>(100, 37, 83);
    check_sort<NTL::OwningStack, NodeL1>(1001, 372, 854);

    std::printf("OK\n");
}


struct NodeL2 : public NTL::SimpleListNode<NodeL2>
{
    int num;

    NodeL2(int n) : num(n)
    {
    }

    NodeL2(const NodeL2 &node) : num(node.num + 100)
    {
    }

    int cmp(const NodeL2 &node) const
    {
        return num - node.num;
    }
};

void test_simple_list_container()
{
    std::printf("\nTesting SimpleList container:\n  constructors... ");

    mem_handler.reset();
    {
        NTL::EmptyAllocator<NodeL2> alloc;  NTL::SimpleList<NodeL2> list(alloc);
        assert(list.empty() && !list && list == false);
        NodeL2 *node = list.first();
        const NodeL2 *cnode = static_cast<const NTL::SimpleList<NodeL2> &>(list).first();
        assert(!node && cnode == node);
        mem_handler.check(0, 0);

        NTL::OwningSimpleList<NodeL2> copy;
        node = copy.first();
        cnode = static_cast<const NTL::OwningSimpleList<NodeL2> &>(copy).first();
        assert(!node && !cnode);
        mem_handler.check(0, 0);

        std::printf("OK\n  prepend(node), take_first(), append(node)... ");

        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL2(9 - i));
        assert(!list.empty() && !!list && list == true);
        mem_handler.check(10, 0);

        node = list.first();
        for(int i = 0; i < 10; i++)
        {
            assert(node->num == i);
            cnode = static_cast<const NodeL2 *>(node)->next();
            node = node->next();
            assert(cnode == node);
        }
        assert(!node);
        mem_handler.check(0, 0);

        for(int i = 0; i < 10; i++)
        {
            node = list.take_first();
            assert(node->num == i);
            delete node;
        }
        node = list.take_first();
        assert(!node);
        mem_handler.check(0, 10);

        for(int i = 0; i < 10; i++)list.append(new_nt NodeL2(i));
        mem_handler.check(10, 0);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  copy(), swap()... ");

        mem_handler.make_fail(10);
        bool res = copy.copy(list);
        assert(!res && !copy.first());
        mem_handler.make_reliable();
        mem_handler.check(9, 9);

        res = copy.copy(list);
        assert(res && copy.first());
        mem_handler.check(10, 0);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        NTL::OwningSimpleList<NodeL2> copy1;
        res = copy1.copy(copy);
        assert(res && copy1.first());
        mem_handler.check(10, 0);

        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        mem_handler.check(0, 0);

        swap(copy, copy1);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  prepend(list), append(list)... ");

        NTL::OwningSimpleList<NodeL2> copy2;
        assert(copy2.empty() && !copy2.first());
        mem_handler.check(0, 0);

        copy.prepend(copy2);
        assert(copy2.empty() && !copy2.first());
        mem_handler.check(0, 0);

        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        mem_handler.check(0, 0);

        copy.append(copy2);
        assert(copy2.empty() && !copy2.first());
        mem_handler.check(0, 0);

        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        mem_handler.check(0, 0);

        copy2.prepend(copy1);
        assert(copy1.empty() && !copy1.first());
        mem_handler.check(0, 0);

        node = copy2.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        copy1.append(copy2);
        assert(copy2.empty() && !copy2.first());
        mem_handler.check(0, 0);

        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        res = copy2.copy(copy);
        assert(res && !copy2.empty());
        mem_handler.check(10, 0);

        node = copy2.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 300);
        mem_handler.check(0, 0);

        copy.prepend(copy1);
        assert(copy1.empty() && !copy1.first());
        mem_handler.check(0, 0);

        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        mem_handler.check(0, 0);

        copy.append(copy2);
        assert(copy2.empty() && !copy2.first());
        mem_handler.check(0, 0);

        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 300);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  prepend_copy(), append_copy()... ");

        res = copy1.prepend_copy(list);
        assert(res && !copy1.empty());
        mem_handler.check(10, 0);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        mem_handler.make_fail(10);
        res = copy1.prepend_copy(list);
        assert(!res && !copy1.empty());
        mem_handler.make_reliable();
        mem_handler.check(9, 9);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        res = copy1.append_copy(copy1);
        assert(res && !copy1.empty());
        mem_handler.check(10, 0);

        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        mem_handler.check(0, 0);

        mem_handler.make_fail(10);
        res = copy1.append_copy(list);
        assert(!res && !copy1.empty());
        mem_handler.make_reliable();
        mem_handler.check(9, 9);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  insert(), take()... ");

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
        {
            NodeL2 *next = list.take(node);
            if(i < 9)
            {
                assert(next->num == i + 1);
                list.insert(next, node);
            }
            else assert(!next);
        }
        assert(!node);
        mem_handler.check(0, 0);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  clear(), destructors... ");

        copy1.clear();
        assert(copy1.empty());
        mem_handler.check(0, 20);

        while((node = list.take_first()))delete node;
        mem_handler.check(0, 10);
    }
    mem_handler.check(0, 30);

    std::printf("OK\n  sort()... ");

    check_sort<NTL::OwningSimpleList, NodeL2>(0, 0, 0);
    check_sort<NTL::OwningSimpleList, NodeL2>(1, 0, 0);
    check_sort<NTL::OwningSimpleList, NodeL2>(2, 1, 0);
    check_sort<NTL::OwningSimpleList, NodeL2>(2, 1, 1);
    check_sort<NTL::OwningSimpleList, NodeL2>(3, 1, 0);
    check_sort<NTL::OwningSimpleList, NodeL2>(3, 1, 1);
    check_sort<NTL::OwningSimpleList, NodeL2>(3, 1, 2);
    check_sort<NTL::OwningSimpleList, NodeL2>(3, 2, 2);
    check_sort<NTL::OwningSimpleList, NodeL2>(3, 2, 1);
    check_sort<NTL::OwningSimpleList, NodeL2>(3, 2, 0);
    check_sort<NTL::OwningSimpleList, NodeL2>(4, 3, 3);
    check_sort<NTL::OwningSimpleList, NodeL2>(27, 8, 11);
    check_sort<NTL::OwningSimpleList, NodeL2>(64, 17, 47);
    check_sort<NTL::OwningSimpleList, NodeL2>(65, 48, 20);
    check_sort<NTL::OwningSimpleList, NodeL2>(100, 37, 83);
    check_sort<NTL::OwningSimpleList, NodeL2>(1001, 372, 854);

    std::printf("OK\n");
}


struct NodeL3 : public NTL::ListNode<NodeL3>
{
    int num;

    NodeL3(int n) : num(n)
    {
    }

    NodeL3(const NodeL3 &node) : num(node.num + 100)
    {
    }

    int cmp(const NodeL3 &node) const
    {
        return num - node.num;
    }
};

void test_list_container()
{
    std::printf("\nTesting List container:\n  constructors... ");

    mem_handler.reset();
    {
        NTL::EmptyAllocator<NodeL3> alloc;  NTL::List<NodeL3> list(alloc);
        assert(list.empty() && !list && list == false);
        NodeL3 *node = list.first();
        const NodeL3 *cnode = static_cast<const NTL::List<NodeL3> &>(list).first();
        assert(!node && cnode == node);
        mem_handler.check(0, 0);

        NTL::OwningList<NodeL3> copy;
        node = copy.first();
        cnode = static_cast<const NTL::OwningList<NodeL3> &>(copy).first();
        assert(!node && !cnode);
        mem_handler.check(0, 0);

        std::printf("OK\n  prepend(), take_first()... ");

        for(int i = 0; i < 10; i++)
        {
            node = new_nt NodeL3(9 - i);
            assert(!node->assigned());
            list.prepend(node);
        }
        assert(!list.empty() && !!list && list == true);
        mem_handler.check(10, 0);

        node = list.first();
        for(int i = 0; i < 10; i++)
        {
            assert(node->assigned() && node->num == i);
            cnode = static_cast<const NodeL3 *>(node)->next();
            node = node->next();
            assert(cnode == node);
        }
        assert(!node);
        mem_handler.check(0, 0);

        for(int i = 0; i < 10; i++)
        {
            node = list.take_first();
            assert(node->num == i);
            delete node;
        }
        node = list.take_first();
        assert(!node);
        mem_handler.check(0, 10);

        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL3(9 - i));
        mem_handler.check(10, 0);

        std::printf("OK\n  copy(), swap()... ");

        mem_handler.make_fail(10);
        bool res = copy.copy(list);
        assert(!res && !copy.first());
        mem_handler.make_reliable();
        mem_handler.check(9, 9);

        res = copy.copy(list);
        assert(res && copy.first());
        mem_handler.check(10, 0);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        NTL::OwningList<NodeL3> copy1;
        res = copy1.copy(copy);
        assert(res && copy1.first());
        mem_handler.check(10, 0);

        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        mem_handler.check(0, 0);

        swap(copy, copy1);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  insert_next(), take_next()... ");

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
        {
            NodeL3 *next = node->take_next();
            if(i < 9)
            {
                assert(!next->assigned() && next->num == i + 1);
                node->insert_next(next);
                assert(next->assigned());
            }
            else assert(!next);
        }
        assert(!node);
        mem_handler.check(0, 0);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  insert_prev(), remove()... ");

        node = list.first();
        for(int i = 0; i < 9; i++, node = node->next())
        {
            NodeL3 *next = node->next();  node->remove();
            assert(!node->assigned());
            next->insert_prev(node);
            assert(node->assigned() && node->next() == next);
        }
        assert(!node->next());
        mem_handler.check(0, 0);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  clear(), destructors... ");

        copy1.clear();
        assert(copy1.empty());
        mem_handler.check(0, 10);

        while((node = list.first()))delete node;
        mem_handler.check(0, 10);
    }
    mem_handler.check(0, 10);

    std::printf("OK\n  sort()... ");

    check_sort<NTL::OwningList, NodeL3>(0, 0, 0);
    check_sort<NTL::OwningList, NodeL3>(1, 0, 0);
    check_sort<NTL::OwningList, NodeL3>(2, 1, 0);
    check_sort<NTL::OwningList, NodeL3>(2, 1, 1);
    check_sort<NTL::OwningList, NodeL3>(3, 1, 0);
    check_sort<NTL::OwningList, NodeL3>(3, 1, 1);
    check_sort<NTL::OwningList, NodeL3>(3, 1, 2);
    check_sort<NTL::OwningList, NodeL3>(3, 2, 2);
    check_sort<NTL::OwningList, NodeL3>(3, 2, 1);
    check_sort<NTL::OwningList, NodeL3>(3, 2, 0);
    check_sort<NTL::OwningList, NodeL3>(4, 3, 3);
    check_sort<NTL::OwningList, NodeL3>(27, 8, 11);
    check_sort<NTL::OwningList, NodeL3>(64, 17, 47);
    check_sort<NTL::OwningList, NodeL3>(65, 48, 20);
    check_sort<NTL::OwningList, NodeL3>(100, 37, 83);
    check_sort<NTL::OwningList, NodeL3>(1001, 372, 854);

    std::printf("OK\n");
}


void test_list()
{
    test_stack_container();
    test_simple_list_container();
    test_list_container();
}
