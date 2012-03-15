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
#include "pointer.h"
#include <cstdio>



struct Node
{
    int test()
    {
        return 12345;
    }
};


void test_pointer_class()
{
    std::printf("\nTesting Pointer class:\n  constructors... ");

    mem_handler.count = 0;
    {
        NTL::Pointer<int> null;
        assert(!null.ptr());
        assert(!mem_handler.count);

        mem_handler.make_reliable();
        NTL::Pointer<int> ptr(new_nt int(123));
        assert(ptr.ptr() && *ptr.ptr() == 123);
        assert(mem_handler.count == 1);

        std::printf("OK\n  destructors... ");
    }
    assert(!mem_handler.count);

    std::printf("OK\n  attach(), detach()... ");
    {
        int *data = new_nt int(555);
        assert(mem_handler.count == 1);

        mem_handler.make_fail(1);
        NTL::Pointer<int> ptr;
        int *res = ptr.attach(data);
        assert(ptr.ptr() == data && res == data);
        assert(mem_handler.count == 1);

        res = ptr.detach();
        assert(!ptr.ptr() && res == data);
        assert(mem_handler.count == 1);

        std::printf("OK\n  set(), clear()... ");

        res = ptr.set(data);
        assert(ptr.ptr() == data && res == data);
        assert(mem_handler.count == 1);

        res = ptr.set(data);
        assert(ptr.ptr() == data && *ptr.ptr() == 555 && res == data);
        assert(mem_handler.count == 1);

        mem_handler.make_reliable();
        data = new_nt int(666);
        assert(mem_handler.count == 2);

        res = ptr.set(data);
        assert(ptr.ptr() == data && *ptr.ptr() == 666 && res == data);
        assert(mem_handler.count == 1);

        ptr.clear();
        assert(!ptr.ptr());
        assert(!mem_handler.count);

        res = ptr.set(new_nt int(333));
        assert(ptr.ptr() && *ptr.ptr() == 333 && res == ptr.ptr());
        assert(mem_handler.count == 1);

        std::printf("OK\n  copy, swap... ");

        data = new_nt int(777);
        assert(mem_handler.count == 2);

        mem_handler.make_fail(1);  ptr = data;
        assert(ptr.ptr() == data && *ptr.ptr() == 777);
        assert(mem_handler.count == 1);

        mem_handler.make_reliable();
        NTL::Pointer<int> ptr1(new_nt int(888));
        assert(ptr1.ptr() && *ptr1.ptr() == 888);
        assert(mem_handler.count == 2);

        mem_handler.make_fail(1);  swap(ptr, ptr1);
        assert(ptr.ptr() && *ptr.ptr() == 888);
        assert(ptr1.ptr() == data && *ptr1.ptr() == 777);
        assert(mem_handler.count == 2);
    }
    assert(!mem_handler.count);

    std::printf("OK\n  conversions... ");
    {
        mem_handler.make_reliable();
        NTL::Pointer<Node> ptr(new_nt Node);  Node *data = ptr;
        assert(ptr.ptr() && data == ptr.ptr());
        assert(ptr->test() == 12345 && (*ptr).test() == 12345);
        assert(mem_handler.count == 1);

        std::printf("OK\n  memory problems... ");

        mem_handler.make_fail(1);
        NTL::Pointer<int> empty(new_nt int(321));
        assert(!empty.ptr());
        assert(mem_handler.count == 1);
    }
    assert(!mem_handler.count);

    std::printf("OK\n");
}


void test_array_class()
{
    std::printf("\nTesting Array class:\n  constructors... ");

    mem_handler.count = 0;
    {
        NTL::Array<int> null;
        assert(!null.ptr());
        assert(!mem_handler.count);

        mem_handler.make_reliable();
        NTL::Array<int> arr1(5);
        assert(arr1.ptr());
        assert(mem_handler.count == 1);

        int *data = new_nt int[3];
        assert(mem_handler.count == 2);
        data[0] = 111;  data[1] = 222;  data[2] = 333;

        mem_handler.make_fail(1);
        NTL::Array<int> arr2(data);
        assert(arr2.ptr() == data);
        assert(mem_handler.count == 2);

        std::printf("OK\n  destructors... ");
    }
    assert(!mem_handler.count);

    std::printf("OK\n  attach(), detach()... ");
    {
        mem_handler.make_reliable();
        int *data = new_nt int[4];
        assert(mem_handler.count == 1);
        data[0] = 0;  data[1] = 111;  data[2] = 222;  data[3] = 333;

        mem_handler.make_fail(1);
        NTL::Array<int> arr1;
        int *res = arr1.attach(data);
        assert(arr1.ptr() == data);
        assert(mem_handler.count == 1);

        res = arr1.detach();
        assert(!arr1.ptr() && res == data);
        assert(mem_handler.count == 1);

        std::printf("OK\n  alloc(), clear()... ");

        NTL::Array<int> arr2;
        mem_handler.make_reliable();
        arr2.alloc(100);
        assert(arr2.ptr());
        assert(mem_handler.count == 2);

        mem_handler.make_fail(1);
        arr2.clear();
        assert(!arr2.ptr());
        assert(mem_handler.count == 1);

        std::printf("OK\n  copy, swap... ");

        arr2 = data;
        assert(arr2.ptr() == data && arr2.ptr()[3] == 333);
        assert(mem_handler.count == 1);

        mem_handler.make_reliable();
        arr1.alloc(100);
        assert(arr1.ptr());
        assert(mem_handler.count == 2);

        mem_handler.make_fail(1);  swap(arr1, arr2);
        assert(arr1.ptr() == data && arr1.ptr()[3] == 333 && arr2.ptr());
        assert(mem_handler.count == 2);

        std::printf("OK\n  conversions... ");

        res = arr1;
        assert(res == data && arr1[3] == 333);
        assert(mem_handler.count == 2);

        std::printf("OK\n  memory problems... ");

        mem_handler.make_fail(1);
        NTL::Array<int> empty(100);
        assert(!empty.ptr());
        assert(mem_handler.count == 2);

        empty.alloc(100);
        assert(!empty.ptr());
        assert(mem_handler.count == 2);
    }
    assert(!mem_handler.count);

    std::printf("OK\n");
}


void test_pointer()
{
    test_pointer_class();
    test_array_class();
}
