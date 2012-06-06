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

    mem_handler.reset();
    {
        NTL::Pointer<int> null;
        assert(!null.ptr());
        mem_handler.check(0, 0);

        NTL::Pointer<int> ptr(new_nt int(123));
        assert(ptr.ptr() && *ptr.ptr() == 123);
        mem_handler.check(1, 0);

        std::printf("OK\n  destructors... ");
    }
    mem_handler.check(0, 1);

    std::printf("OK\n  attach(), detach()... ");
    {
        int *data = new_nt int(555);
        mem_handler.check(1, 0);

        NTL::Pointer<int> ptr;
        int *res = ptr.attach(data);
        assert(ptr.ptr() == data && res == data);
        mem_handler.check(0, 0);

        res = ptr.detach();
        assert(!ptr.ptr() && res == data);
        mem_handler.check(0, 0);

        std::printf("OK\n  set(), clear()... ");

        res = ptr.set(data);
        assert(ptr.ptr() == data && res == data);
        mem_handler.check(0, 0);

        data = new_nt int(666);
        mem_handler.check(1, 0);

        res = ptr.set(data);
        assert(ptr.ptr() == data && *ptr.ptr() == 666 && res == data);
        mem_handler.check(0, 1);

        ptr.clear();
        assert(!ptr.ptr());
        mem_handler.check(0, 1);

        res = ptr.set(new_nt int(333));
        assert(ptr.ptr() && *ptr.ptr() == 333 && res == ptr.ptr());
        mem_handler.check(1, 0);

        std::printf("OK\n  copy, swap... ");

        data = new_nt int(777);
        mem_handler.check(1, 0);

        ptr = data;
        assert(ptr.ptr() == data && *ptr.ptr() == 777);
        mem_handler.check(0, 1);

        NTL::Pointer<int> ptr1(new_nt int(888));
        assert(ptr1.ptr() && *ptr1.ptr() == 888);
        mem_handler.check(1, 0);

        swap(ptr, ptr1);
        assert(ptr.ptr() && *ptr.ptr() == 888);
        assert(ptr1.ptr() == data && *ptr1.ptr() == 777);
        mem_handler.check(0, 0);
    }
    mem_handler.check(0, 2);

    std::printf("OK\n  conversions... ");
    {
        NTL::Pointer<Node> ptr(new_nt Node);  Node *data = ptr;
        assert(ptr.ptr() && data == ptr.ptr());
        assert(ptr->test() == 12345 && (*ptr).test() == 12345);
        mem_handler.check(1, 0);

        std::printf("OK\n  memory problems... ");

        mem_handler.make_fail(1);
        NTL::Pointer<int> empty(new_nt int(321));
        assert(!empty.ptr());
        mem_handler.make_reliable();
        mem_handler.check(0, 0);
    }
    mem_handler.check(0, 1);

    std::printf("OK\n");
}


void test_array_class()
{
    std::printf("\nTesting Array class:\n  constructors... ");

    mem_handler.reset();
    {
        NTL::Array<int> null;
        assert(!null.ptr());
        mem_handler.check(0, 0);

        NTL::Array<int> arr1(5);
        assert(arr1.ptr());
        mem_handler.check(1, 0);

        int *data = new_nt int[3];
        mem_handler.check(1, 0);
        data[0] = 111;  data[1] = 222;  data[2] = 333;

        NTL::Array<int> arr2(data);
        assert(arr2.ptr() == data);
        mem_handler.check(0, 0);

        std::printf("OK\n  destructors... ");
    }
    mem_handler.check(0, 2);

    std::printf("OK\n  attach(), detach()... ");
    {
        int *data = new_nt int[4];
        mem_handler.check(1, 0);
        data[0] = 0;  data[1] = 111;  data[2] = 222;  data[3] = 333;

        NTL::Array<int> arr1;
        int *res = arr1.attach(data);
        assert(arr1.ptr() == data);
        mem_handler.check(0, 0);

        res = arr1.detach();
        assert(!arr1.ptr() && res == data);
        mem_handler.check(0, 0);

        std::printf("OK\n  alloc(), set(), clear()... ");

        NTL::Array<int> arr2;
        arr2.alloc(100);
        assert(arr2.ptr());
        mem_handler.check(1, 0);

        int *data1 = new_nt int[100];
        res = arr2.set(data1);
        assert(arr2.ptr() == data1 && res == data1);
        mem_handler.check(1, 1);

        arr2.clear();
        assert(!arr2.ptr());
        mem_handler.check(0, 1);

        std::printf("OK\n  copy, swap... ");

        arr2 = data;
        assert(arr2.ptr() == data && arr2.ptr()[3] == 333);
        mem_handler.check(0, 0);

        arr1.alloc(100);
        assert(arr1.ptr());
        mem_handler.check(1, 0);

        swap(arr1, arr2);
        assert(arr1.ptr() == data && arr1.ptr()[3] == 333 && arr2.ptr());
        mem_handler.check(0, 0);

        std::printf("OK\n  conversions... ");

        res = arr1;
        assert(res == data && arr1[3] == 333);
        mem_handler.check(0, 0);

        std::printf("OK\n  memory problems... ");

        mem_handler.make_fail(1);
        NTL::Array<int> empty(100);
        assert(!empty.ptr());
        mem_handler.check(0, 0);

        empty.alloc(100);
        assert(!empty.ptr());
        mem_handler.make_reliable();
        mem_handler.check(0, 0);
    }
    mem_handler.check(0, 2);

    std::printf("OK\n");
}


void test_pointer()
{
    test_pointer_class();
    test_array_class();
}
