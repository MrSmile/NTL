/***************************************************************************************************
* Nothrow Template Library : pointer.cpp : test suite for pointer.h
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



struct NodeL1 : public NTL::StackNode<NodeL1>
{
    int num;

    NodeL1(int n) : num(n)
    {
    }

    NodeL1(const NodeL1 &node) : num(node.num + 100)
    {
    }
};

void test_stack_container()
{
    std::printf("\nTesting Stack container:\n  constructors... ");

    mem_handler.count = 0;
    {
        NTL::EmptyAllocator<NodeL1> alloc;  NTL::Stack<NodeL1> list(alloc);
        assert(!list.not_empty() && !list && list == false);
        NodeL1 *node = list.first();
        const NodeL1 *cnode = static_cast<const NTL::Stack<NodeL1> &>(list).first();
        assert(!node && cnode == node);
        assert(!mem_handler.count);

        NTL::OwningStack<NodeL1> copy;
        node = copy.first();
        cnode = static_cast<const NTL::OwningStack<NodeL1> &>(copy).first();
        assert(!node && !cnode);
        assert(!mem_handler.count);

        std::printf("OK\n  prepend(), take_first()... ");

        mem_handler.make_reliable();
        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL1(9 - i));
        assert(list.not_empty() && !!list && list == true);
        assert(mem_handler.count == 10);

        mem_handler.make_fail(1);
        node = list.first();
        for(int i = 0; i < 10; i++)
        {
            assert(node->num == i);
            cnode = static_cast<const NodeL1 *>(node)->next();
            node = node->next();
            assert(cnode == node);
        }
        assert(!node);
        assert(mem_handler.count == 10);

        for(int i = 0; i < 10; i++)
        {
            node = list.take_first();
            assert(node->num == i);
            delete node;
        }
        node = list.take_first();
        assert(!node);
        assert(!mem_handler.count);

        mem_handler.make_reliable();
        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL1(9 - i));
        assert(mem_handler.count == 10);

        std::printf("OK\n  copy(), swap()... ");

        mem_handler.make_fail(10);
        bool res = copy.copy(list);
        assert(!res && !copy.first());
        assert(mem_handler.count == 10);

        mem_handler.make_reliable();
        res = copy.copy(list);
        assert(res && copy.first());
        assert(mem_handler.count == 20);

        mem_handler.make_fail(1);
        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        assert(mem_handler.count == 20);

        mem_handler.make_reliable();
        NTL::OwningStack<NodeL1> copy1;
        res = copy1.copy(copy);
        assert(res && copy1.first());
        assert(mem_handler.count == 30);

        mem_handler.make_fail(1);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        assert(mem_handler.count == 30);

        swap(copy, copy1);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        assert(mem_handler.count == 30);

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
        assert(mem_handler.count == 30);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        assert(mem_handler.count == 30);

        std::printf("OK\n  clear(), destructors... ");

        copy1.clear();
        assert(!copy1.not_empty());
        assert(mem_handler.count == 20);

        while((node = list.take_first()))delete node;
        assert(mem_handler.count == 10);
    }
    assert(!mem_handler.count);

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
};

void test_simple_list_container()
{
    std::printf("\nTesting SimpleList container:\n  constructors... ");

    mem_handler.count = 0;
    {
        NTL::EmptyAllocator<NodeL2> alloc;  NTL::SimpleList<NodeL2> list(alloc);
        assert(!list.not_empty() && !list && list == false);
        NodeL2 *node = list.first();
        const NodeL2 *cnode = static_cast<const NTL::SimpleList<NodeL2> &>(list).first();
        assert(!node && cnode == node);
        assert(!mem_handler.count);

        NTL::OwningSimpleList<NodeL2> copy;
        node = copy.first();
        cnode = static_cast<const NTL::OwningSimpleList<NodeL2> &>(copy).first();
        assert(!node && !cnode);
        assert(!mem_handler.count);

        std::printf("OK\n  prepend(node), take_first(), append(node)... ");

        mem_handler.make_reliable();
        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL2(9 - i));
        assert(list.not_empty() && !!list && list == true);
        assert(mem_handler.count == 10);

        mem_handler.make_fail(1);
        node = list.first();
        for(int i = 0; i < 10; i++)
        {
            assert(node->num == i);
            cnode = static_cast<const NodeL2 *>(node)->next();
            node = node->next();
            assert(cnode == node);
        }
        assert(!node);
        assert(mem_handler.count == 10);

        for(int i = 0; i < 10; i++)
        {
            node = list.take_first();
            assert(node->num == i);
            delete node;
        }
        node = list.take_first();
        assert(!node);
        assert(!mem_handler.count);

        mem_handler.make_reliable();
        for(int i = 0; i < 10; i++)list.append(new_nt NodeL2(i));
        assert(mem_handler.count == 10);

        mem_handler.make_fail(1);
        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        assert(mem_handler.count == 10);

        std::printf("OK\n  copy(), swap()... ");

        mem_handler.make_fail(10);
        bool res = copy.copy(list);
        assert(!res && !copy.first());
        assert(mem_handler.count == 10);

        mem_handler.make_reliable();
        res = copy.copy(list);
        assert(res && copy.first());
        assert(mem_handler.count == 20);

        mem_handler.make_fail(1);
        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        assert(mem_handler.count == 20);

        mem_handler.make_reliable();
        NTL::OwningSimpleList<NodeL2> copy1;
        res = copy1.copy(copy);
        assert(res && copy1.first());
        assert(mem_handler.count == 30);

        mem_handler.make_fail(1);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        assert(mem_handler.count == 30);

        swap(copy, copy1);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        assert(mem_handler.count == 30);

        std::printf("OK\n  prepend(list), append(list)... ");

        NTL::OwningSimpleList<NodeL2> copy2;
        assert(!copy2.not_empty() && !copy2.first());
        assert(mem_handler.count == 30);

        copy.prepend(copy2);
        assert(!copy2.not_empty() && !copy2.first());
        assert(mem_handler.count == 30);

        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        assert(mem_handler.count == 30);

        copy.append(copy2);
        assert(!copy2.not_empty() && !copy2.first());
        assert(mem_handler.count == 30);

        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        assert(mem_handler.count == 30);

        copy2.prepend(copy1);
        assert(!copy1.not_empty() && !copy1.first());
        assert(mem_handler.count == 30);

        node = copy2.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        assert(mem_handler.count == 30);

        copy1.append(copy2);
        assert(!copy2.not_empty() && !copy2.first());
        assert(mem_handler.count == 30);

        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        assert(mem_handler.count == 30);

        mem_handler.make_reliable();
        res = copy2.copy(copy);
        assert(res && copy2.not_empty());
        assert(mem_handler.count == 40);

        mem_handler.make_fail(1);
        node = copy2.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 300);
        assert(mem_handler.count == 40);

        copy.prepend(copy1);
        assert(!copy1.not_empty() && !copy1.first());
        assert(mem_handler.count == 40);

        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        assert(mem_handler.count == 40);

        copy.append(copy2);
        assert(!copy2.not_empty() && !copy2.first());
        assert(mem_handler.count == 40);

        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 300);  assert(!node);
        assert(mem_handler.count == 40);

        std::printf("OK\n  prepend_copy(), append_copy()... ");

        mem_handler.make_reliable();
        res = copy1.prepend_copy(list);
        assert(res && copy1.not_empty());
        assert(mem_handler.count == 50);

        mem_handler.make_fail(1);
        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        assert(mem_handler.count == 50);

        mem_handler.make_fail(10);
        res = copy1.prepend_copy(list);
        assert(!res && copy1.not_empty());
        assert(mem_handler.count == 50);

        mem_handler.make_fail(1);
        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);  assert(!node);
        assert(mem_handler.count == 50);

        mem_handler.make_reliable();
        res = copy1.append_copy(copy1);
        assert(res && copy1.not_empty());
        assert(mem_handler.count == 60);

        mem_handler.make_fail(1);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        assert(mem_handler.count == 60);

        mem_handler.make_fail(10);
        res = copy1.append_copy(list);
        assert(!res && copy1.not_empty());
        assert(mem_handler.count == 60);

        mem_handler.make_fail(1);
        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 100);
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i + 200);  assert(!node);
        assert(mem_handler.count == 60);

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
        assert(mem_handler.count == 60);

        node = list.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->num == i);  assert(!node);
        assert(mem_handler.count == 60);

        std::printf("OK\n  clear(), destructors... ");

        copy1.clear();
        assert(!copy1.not_empty());
        assert(mem_handler.count == 40);

        while((node = list.take_first()))delete node;
        assert(mem_handler.count == 30);
    }
    assert(!mem_handler.count);

    std::printf("OK\n");
}


struct NodeL3 : public NTL::ListNode<NodeL3>
{
    int num;

    NodeL3(int n) : num(n)
    {
    }

    NodeL3(const NodeL3 &node) : num(node.num)
    {
    }
};

void test_list_container()
{
    std::printf("\nTesting List container:\n  constructors... ");

    mem_handler.count = 0;
    {
        // TODO

        std::printf("OK\n  destructors... ");
    }
    assert(!mem_handler.count);

    std::printf("OK\n");
}


void test_list()
{
    test_stack_container();
    test_simple_list_container();
    //test_list_container();
}
