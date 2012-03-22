/***************************************************************************************************
* Nothrow Template Library : tree.cpp : test suite for tree.h
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
#include "tree.h"
#include <cstdio>



struct NodeT1 : public NTL::TreeNode<NodeT1>, public NTL::SimpleKey<int>
{
    NodeT1(int n) : SimpleKey<int>(n)
    {
    }

    NodeT1(const NodeT1 &node) : SimpleKey<int>(node.id() + 100)
    {
    }
};

void test_tree_container()
{
    std::printf("\nTesting Tree container:\n  constructors... ");

    mem_handler.reset();
    {
        NTL::EmptyAllocator<NodeT1> alloc;  NTL::Tree<NodeT1> tree(alloc);
        assert(!tree.not_empty() && !tree && tree == false);
        NodeT1 *node = tree.first();
        const NodeT1 *cnode = static_cast<const NTL::Tree<NodeT1> &>(tree).first();
        assert(!node && cnode == node);
        node = tree.last();
        cnode = static_cast<const NTL::Tree<NodeT1> &>(tree).last();
        assert(!node && cnode == node);
        mem_handler.check(0, 0);

        NTL::OwningTree<NodeT1> copy;
        node = copy.first();
        cnode = static_cast<const NTL::OwningTree<NodeT1> &>(copy).first();
        assert(!node && !cnode);
        node = tree.last();
        cnode = static_cast<const NTL::OwningTree<NodeT1> &>(copy).last();
        assert(!node && cnode == node);
        mem_handler.check(0, 0);

        std::printf("OK\n  find_place(), insert(), take(), find()... ");

        for(int i = 0; i < 10; i++)
        {
            int k = (7 * i + 5) % 10;  k = 2 * k + 1;
            NTL::Tree<NodeT1>::ConstPlace place = tree.find_place(k);
            assert(!place.node());
            tree.insert(new_nt NodeT1(k), place);
        }
        assert(tree.not_empty() && !!tree && tree == true);
        mem_handler.check(10, 0);

        for(int i = 0; i <= 20; i++)
        {
            NTL::Tree<NodeT1>::Place place = tree.find_place(i);
            NTL::Tree<NodeT1>::ConstPlace cplace = static_cast<const NTL::Tree<NodeT1> &>(tree).find_place(i);
            if(i & 1)
            {
                assert(place.node()->id() == i && cplace.node()->id() == i);
                assert(place.before() == place.node() && place.after() == place.node());
                assert(cplace.before() == cplace.node() && cplace.after() == cplace.node());
            }
            else
            {
                assert(!place.node() && !cplace.node());
                if(i)assert(place.before()->id() == i - 1 && cplace.before()->id() == i - 1);
                else assert(!place.before() && !cplace.before());
                if(i < 20)assert(place.after()->id() == i + 1 && cplace.after()->id() == i + 1);
                else assert(!place.after() && !cplace.after());
            }
        }
        mem_handler.check(0, 0);

        tree.check_();
        node = tree.first();
        for(int i = 0; i < 10; i++, node = node->next())
        {
            int k = 2 * i + 1;
            assert(node->assigned() && node->id() == k);

            NodeT1 *ptr = node->prev();
            assert(i ? ptr->id() == k - 2 : !ptr);
            cnode = static_cast<const NodeT1 *>(node)->prev();
            assert(cnode == ptr);

            ptr = node->next();
            assert(i < 9 ? ptr->id() == k + 2 : !ptr);
            cnode = static_cast<const NodeT1 *>(node)->next();
            assert(cnode == ptr);
        }
        assert(!node);
        node = tree.last();
        for(int i = 0; i < 10; i++, node = node->prev())
            assert(node->assigned() && node->id() == 19 - 2 * i);  assert(!node);
        mem_handler.check(0, 0);

        for(int i = 0; i <= 20; i++)
        {
            node = tree.take(i);
            if(i & 1)
            {
                assert(!node->assigned() && node->id() == i);
                delete node;
            }
            else assert(!node);
        }
        tree.check_();
        assert(!tree.not_empty());
        mem_handler.check(0, 10);

        for(int i = 0; i < 10; i++)
            tree.insert(new_nt NodeT1(2 * ((7 * i + 5) % 10) + 1));
        assert(tree.not_empty() && !!tree && tree == true);
        mem_handler.check(10, 0);

        tree.check_();
        node = tree.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 1);  assert(!node);
        node = tree.last();
        for(int i = 0; i < 10; i++, node = node->prev())
            assert(node->assigned() && node->id() == 19 - 2 * i);  assert(!node);
        mem_handler.check(0, 0);

        for(int i = 0; i <= 20; i++)
        {
            node = tree.find(i);
            cnode = static_cast<const NTL::Tree<NodeT1> &>(tree).find(i);
            assert(cnode == node);
            if(i & 1)assert(node->assigned() && node->id() == i);
            else assert(!node);
        }
        mem_handler.check(0, 0);

        std::printf("OK\n  copy(), swap()... ");

        mem_handler.make_fail(10);
        bool res = copy.copy(tree);
        copy.check_();
        assert(!res && !copy.not_empty());
        mem_handler.make_reliable();
        mem_handler.check(9, 9);

        res = copy.copy(tree);
        assert(res && copy.not_empty());
        mem_handler.check(10, 0);

        copy.check_();
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 101);  assert(!node);
        mem_handler.check(0, 0);

        NTL::OwningTree<NodeT1> copy1;
        res = copy1.copy(copy);
        assert(res && copy1.not_empty());
        mem_handler.check(10, 0);

        copy1.check_();
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 201);  assert(!node);
        mem_handler.check(0, 0);

        swap(copy, copy1);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 201);  assert(!node);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 101);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  get(), remove(), delete... ");

        for(int i = 0; i <= 20; i++)
        {
            node = copy.get(i + 200);
            assert(node->assigned() && node->id() == i + 200);
            mem_handler.check(i & 1 ? 0 : 1, 0);
        }

        copy.check_();
        node = copy.first();
        for(int i = 0; i <= 20; i++, node = node->next())
            assert(node->assigned() && node->id() == i + 200);  assert(!node);
        mem_handler.check(0, 0);

        for(int i = 0; i < 10; i++)delete tree.find(2 * ((3 * i + 7) % 10) + 1);
        assert(!tree.not_empty() && !tree.first() && !tree.last());
        mem_handler.check(0, 10);

        for(int i = 0; i <= 20; i += 2)
        {
            node = copy.find(i + 200);
            assert(node->assigned() && node->id() == i + 200);
            node->remove();
            assert(!node->assigned());
            delete node;
        }
        mem_handler.check(0, 11);

        copy.check_();
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 201);  assert(!node);
        mem_handler.check(0, 0);

        std::printf("OK\n  clear(), destructors... ");

        copy1.clear();
        copy1.check_();
        assert(!copy1.not_empty());
        mem_handler.check(0, 10);
    }
    mem_handler.check(0, 10);

    std::printf("OK\n");
}


struct NodeT2 : public NTL::IndexerNode<NodeT2>, public NTL::SimpleKey<int>
{
    NodeT2(int n) : SimpleKey<int>(n)
    {
    }

    NodeT2(const NodeT2 &node) : SimpleKey<int>(node.id() + 100)
    {
    }
};

void test_indexer_container()
{
    std::printf("\nTesting Indexer container:\n  constructors... ");

    mem_handler.reset();
    {
        NTL::EmptyAllocator<NodeT2> alloc;  NTL::Indexer<NodeT2> tree(alloc);
        assert(!tree.not_empty() && !tree && tree == false && !tree.size());
        NodeT2 *node = tree.first();
        const NodeT2 *cnode = static_cast<const NTL::Indexer<NodeT2> &>(tree).first();
        assert(!node && cnode == node);
        node = tree.last();
        cnode = static_cast<const NTL::Indexer<NodeT2> &>(tree).last();
        assert(!node && cnode == node);
        mem_handler.check(0, 0);

        NTL::OwningIndexer<NodeT2> copy;
        node = copy.first();
        cnode = static_cast<const NTL::OwningIndexer<NodeT2> &>(copy).first();
        assert(!node && !cnode);
        node = tree.last();
        cnode = static_cast<const NTL::OwningIndexer<NodeT2> &>(copy).last();
        assert(!node && cnode == node);
        mem_handler.check(0, 0);

        std::printf("OK\n  find_place(), insert(), take(), find()... ");

        for(int i = 0; i < 10; i++)
        {
            int k = (7 * i + 5) % 10;  k = 2 * k + 1;
            NTL::Indexer<NodeT2>::ConstPlace place = tree.find_place(k);
            assert(!place.node());
            tree.insert(new_nt NodeT2(k), place);
        }
        assert(tree.not_empty() && !!tree && tree == true && tree.size() == 10);
        mem_handler.check(10, 0);

        for(int i = 0; i <= 20; i++)
        {
            NTL::Indexer<NodeT2>::Place place = tree.find_place(i);
            NTL::Indexer<NodeT2>::ConstPlace cplace = static_cast<const NTL::Indexer<NodeT2> &>(tree).find_place(i);
            if(i & 1)
            {
                assert(place.node()->id() == i && cplace.node()->id() == i);
                assert(place.before() == place.node() && place.after() == place.node());
                assert(cplace.before() == cplace.node() && cplace.after() == cplace.node());
            }
            else
            {
                assert(!place.node() && !cplace.node());
                if(i)assert(place.before()->id() == i - 1 && cplace.before()->id() == i - 1);
                else assert(!place.before() && !cplace.before());
                if(i < 20)assert(place.after()->id() == i + 1 && cplace.after()->id() == i + 1);
                else assert(!place.after() && !cplace.after());
            }
            assert(place.index() == size_t(i / 2) && cplace.index() == size_t(i / 2));
        }
        mem_handler.check(0, 0);

        tree.check_();
        node = tree.first();
        for(int i = 0; i < 10; i++, node = node->next())
        {
            int k = 2 * i + 1;
            assert(node->assigned() && node->id() == k && node->index() == size_t(i));

            NodeT2 *ptr = node->prev();
            assert(i ? ptr->id() == k - 2 : !ptr);
            cnode = static_cast<const NodeT2 *>(node)->prev();
            assert(cnode == ptr);

            ptr = node->next();
            assert(i < 9 ? ptr->id() == k + 2 : !ptr);
            cnode = static_cast<const NodeT2 *>(node)->next();
            assert(cnode == ptr);
        }
        assert(!node && tree.size() == 10);
        node = tree.last();
        for(int i = 0; i < 10; i++, node = node->prev())
            assert(node->assigned() && node->id() == 19 - 2 * i);  assert(!node);
        mem_handler.check(0, 0);

        for(int i = 0; i <= 20; i++)
        {
            node = tree.take(i);
            if(i & 1)
            {
                assert(!node->assigned() && node->id() == i);
                delete node;
            }
            else assert(!node);
        }
        tree.check_();
        assert(!tree.not_empty() && !tree.size());
        mem_handler.check(0, 10);

        for(int i = 0; i < 10; i++)
            tree.insert(new_nt NodeT2(2 * ((7 * i + 5) % 10) + 1));
        assert(tree.not_empty() && !!tree && tree == true);
        mem_handler.check(10, 0);

        tree.check_();
        node = tree.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 1 && node->index() == size_t(i));
        assert(!node && tree.size() == 10);
        node = tree.last();
        for(int i = 0; i < 10; i++, node = node->prev())
            assert(node->assigned() && node->id() == 19 - 2 * i);  assert(!node);
        mem_handler.check(0, 0);

        for(int i = 0; i <= 20; i++)
        {
            node = tree.find(i);
            cnode = static_cast<const NTL::Indexer<NodeT2> &>(tree).find(i);
            assert(cnode == node);
            if(i & 1)assert(node->assigned() && node->id() == i && node->index() == size_t(i / 2));
            else assert(!node);
        }
        mem_handler.check(0, 0);

        std::printf("OK\n  place_at(), take_at(), insert_at()... ");

        for(int i = 0; i <= 10; i++)
        {
            NTL::Indexer<NodeT2>::Place place = tree.place_at(i);
            NTL::Indexer<NodeT2>::ConstPlace cplace = static_cast<const NTL::Indexer<NodeT2> &>(tree).place_at(i);
            if(i)assert(place.before()->id() == 2 * i - 1 && cplace.before()->id() == 2 * i - 1);
            else assert(!place.before() && !cplace.before());
            if(i < 10)assert(place.after()->id() == 2 * i + 1 && cplace.after()->id() == 2 * i + 1);
            else assert(!place.after() && !cplace.after());
            assert(place.index() == size_t(i) && cplace.index() == size_t(i));
        }
        mem_handler.check(0, 0);

        for(int i = 0; i < 5; i++)
        {
            node = tree.take_at(i);
            assert(!node->assigned() && node->id() == 4 * i + 1);
            delete node;
        }
        mem_handler.check(0, 5);

        tree.check_();
        node = tree.first();
        for(int i = 0; i < 5; i++, node = node->next())
            assert(node->assigned() && node->id() == 4 * i + 3 && node->index() == size_t(i));
        assert(!node && tree.size() == 5);
        mem_handler.check(0, 0);

        for(int i = 0; i < 5; i++)
        {
            node = new_nt NodeT2(4 * i + 1);
            tree.insert_at(node, 2 * i);
            assert(node->assigned() && node->index() == size_t(2 * i));
        }
        mem_handler.check(5, 0);

        tree.check_();
        node = tree.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 1 && node->index() == size_t(i));
        assert(!node && tree.size() == 10);
        mem_handler.check(0, 0);

        std::printf("OK\n  copy(), swap()... ");

        mem_handler.make_fail(10);
        bool res = copy.copy(tree);
        copy.check_();
        assert(!res && !copy.not_empty() && !copy.size());
        mem_handler.make_reliable();
        mem_handler.check(9, 9);

        res = copy.copy(tree);
        assert(res && copy.not_empty());
        mem_handler.check(10, 0);

        copy.check_();
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 101 && node->index() == size_t(i));
        assert(!node && copy.size() == 10);
        mem_handler.check(0, 0);

        NTL::OwningIndexer<NodeT2> copy1;
        res = copy1.copy(copy);
        assert(res && copy1.not_empty());
        mem_handler.check(10, 0);

        copy1.check_();
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 201 && node->index() == size_t(i));
        assert(!node && copy1.size() == 10);
        mem_handler.check(0, 0);

        swap(copy, copy1);
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 201 && node->index() == size_t(i));
        assert(!node && copy.size() == 10);
        node = copy1.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 101 && node->index() == size_t(i));
        assert(!node && copy1.size() == 10);
        mem_handler.check(0, 0);

        std::printf("OK\n  get(), remove(), delete... ");

        for(int i = 0; i <= 20; i++)
        {
            node = copy.get(i + 200);
            assert(node->assigned() && node->id() == i + 200 && node->index() == size_t(i));
            mem_handler.check(i & 1 ? 0 : 1, 0);
        }

        copy.check_();
        node = copy.first();
        for(int i = 0; i <= 20; i++, node = node->next())
            assert(node->assigned() && node->id() == i + 200 && node->index() == size_t(i));
        assert(!node && copy.size() == 21);
        mem_handler.check(0, 0);

        for(int i = 0; i < 10; i++)delete tree.find(2 * ((3 * i + 7) % 10) + 1);
        assert(!tree.not_empty() && !tree.first() && !tree.last() && !tree.size());
        mem_handler.check(0, 10);

        for(int i = 0; i <= 20; i += 2)
        {
            node = copy.find(i + 200);
            assert(node->assigned() && node->id() == i + 200);
            node->remove();
            assert(!node->assigned());
            delete node;
        }
        mem_handler.check(0, 11);

        copy.check_();
        node = copy.first();
        for(int i = 0; i < 10; i++, node = node->next())
            assert(node->assigned() && node->id() == 2 * i + 201 && node->index() == size_t(i));
        assert(!node && copy.size() == 10);
        mem_handler.check(0, 0);

        std::printf("OK\n  clear(), destructors... ");

        copy1.clear();
        copy1.check_();
        assert(!copy1.not_empty() && !copy1.size());
        mem_handler.check(0, 10);
    }
    mem_handler.check(0, 10);

    std::printf("OK\n");
}


void test_tree()
{
    test_tree_container();
    test_indexer_container();
}
