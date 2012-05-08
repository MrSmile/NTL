/***************************************************************************************************
* Nothrow Template Library : literal.cpp : test suite for literal.h
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
#include "literal.h"
#include <cstdio>



void test_literal_class()
{
    const char *str = "string";  size_t len = std::strlen(str);
    std::printf("\nTesting Literal class:\n  constructors... ");

    NTL::Literal null;
    assert(!null.valid() && !null.length() && !null.data());

    NTL::Literal zero("ze\0ro");
    assert(zero.valid() && zero.length() == 5 && !std::memcmp(zero.data(), "ze\0ro", 6));

    char buf[8] = "123";  NTL::Literal nonzero(buf);
    assert(nonzero.valid() && nonzero.length() == 3 && !std::memcmp(nonzero.data(), buf, 3));

    NTL::Literal test(str);
    assert(test.valid() && test.length() == len && !std::memcmp(test.data(), str, len));

    NTL::Literal part(str, 3);
    assert(part.valid() && part.length() == 3 && !std::memcmp(part.data(), str, 3));

    NTL::Literal copy(test);
    assert(copy.valid() && copy.length() == len && !std::memcmp(copy.data(), str, len));

    std::printf("OK\n  operator[], at()... ");

    for(size_t i = 0; i < len; i++)
        assert(test[i] == str[i] && copy.at(i) == str[i]);

    std::printf("OK\n  substr()... ");

    NTL::Literal empty = test.substr(test.length());
    assert(empty.valid() && !empty.length());

    NTL::Literal end = test.substr(3);
    assert(end.valid() && end.length() == len - 3 && !std::memcmp(end.data(), str + 3, len - 3));

    NTL::Literal center = test.substr(1, 3);
    assert(center.valid() && center.length() == 3 && !std::memcmp(center.data(), str + 1, 3));

    std::printf("OK\n  scan(), rscan()... ");

    NTL::Literal find = "search string";
    assert(find.scan('s') == 0 && find.scan('s', 0) == 0 && find.scan('s', 1) == 7);
    assert(find.scan('r') == 3 && find.scan('r', 3) == 3 && find.scan('r', 4) == 9);
    assert(find.rscan('s') == 7 && find.rscan('s', 7) == 7 && find.rscan('s', 6) == 0);
    assert(find.rscan('r') == 9 && find.rscan('r', 9) == 9 && find.rscan('r', 8) == 3);
    assert(find.scan('z') == size_t(-1) && find.scan('z', 3) == size_t(-1));
    assert(find.rscan('z') == size_t(-1) && find.rscan('z', 9) == size_t(-1));

    std::printf("OK\n  comparisons... ");

    assert(NTL::Literal("").cmp(null) > 0 && null == NTL::Literal());
    assert(!part.cmp("str") && part.cmp("sts") < 0 && part.cmp("stq") > 0);
    assert(part.cmp(NTL::Literal("str", 4)) < 0 && part.cmp("stq ") > 0);
    assert(NTL::Literal("str", 4).cmp(part) > 0 && part.cmp("su") < 0);
    assert(part == "str" && part < NTL::Literal("str", 4) && part > "r");
    assert(part <= "str" && part >= static_cast<const char *>("str") && !(part != "str"));

    std::printf("OK\n  general strlen/cmp... ");

    const int data[] = {50, 40, 30, 20, 10, 0};
    typedef NTL::LiteralBase<int> ILiteral;
    ILiteral intstr = data;  len = sizeof(data) / sizeof(int) - 1;
    assert(intstr.valid() && intstr.length() == len);
    assert(!std::memcmp(intstr.data(), data, len * sizeof(int)));

    assert(ILiteral(data + len).cmp(ILiteral()) > 0);
    const int data_p[] = {50, 40, 30, 20, 11, 0}, data_n[] = {50, 40, 30, 20, 9, 0};
    const int data_ps[] = {50, 40, 30, 21, 0}, data_nl[] = {50, 40, 30, 20, 9, 1, 0};
    assert(!intstr.cmp(data) && intstr.cmp(data_p) < 0 && intstr.cmp(data_n) > 0);
    assert(intstr.cmp(ILiteral(data, len + 1)) < 0 && intstr.cmp(data_nl) > 0);
    assert(ILiteral(data, len + 1).cmp(intstr) > 0 && intstr.cmp(data_ps) < 0);
    assert(intstr == data && intstr < ILiteral(data, len + 1) && intstr > data_n);
    assert(intstr <= data && intstr >= data && !(intstr != data));

    std::printf("OK\n");
}


struct DerivedString : public NTL::String
{
    explicit DerivedString(size_t n) : NTL::String(n)
    {
    }

    bool resize(size_t n)
    {
        return NTL::String::resize(n);
    }

    const char *buffer() const
    {
        return NTL::String::buffer();
    }

    char *buffer()
    {
        return NTL::String::buffer();
    }
};


void test_string_class()  // TODO: non ZT
{
    const char *str = "string";  size_t len = std::strlen(str);
    std::printf("\nTesting String class:\n  constructors... ");

    mem_handler.reset();
    {
        NTL::String null;
        assert(!null.valid() && !null.length() && !null.data());
        mem_handler.check(0, 0);

        DerivedString dnull(0);
        assert(!dnull.valid() && !dnull.length() && !dnull.data());
        mem_handler.check(0, 0);

        NTL::String zero("ze\0ro");
        assert(zero.valid() && zero.length() == 5);
        assert(!std::memcmp(zero.data(), "ze\0ro", 6));
        mem_handler.check(1, 0);

        NTL::Literal ltr("str~~~", 3);  NTL::String test(ltr);
        assert(test.valid() && test.length() == 3);
        assert(!std::memcmp(test.data(), ltr.data(), 3) && !test.data()[3]);
        mem_handler.check(1, 0);

        NTL::String test1(str);
        assert(test1.valid() && test1.length() == len);
        assert(!std::memcmp(test1.data(), str, len + 1));
        mem_handler.check(1, 0);

        NTL::String test2(str, 3);
        assert(test2.valid() && test2.length() == 3);
        assert(!std::memcmp(test2.data(), str, 3) && !test2.data()[3]);
        mem_handler.check(1, 0);

        NTL::String copy(test1);
        assert(copy.valid() && copy.length() == len);
        assert(!std::memcmp(copy.data(), str, len + 1));
        mem_handler.check(0, 0);

        DerivedString dtest(len + 1);
        assert(dtest.buffer() == dtest.data());
        assert(dtest.valid() && dtest.length() == len);
        std::memcpy(dtest.buffer(), str, len + 1);
        assert(!std::memcmp(dtest.data(), str, len + 1));
        mem_handler.check(1, 0);

        std::printf("OK\n  general strlen... ");

        const int data[] = {5, 4, 3, 2, 1, 0};
        NTL::StringBase<int> intstr(data);  len = sizeof(data) / sizeof(int);
        assert(intstr.valid() && intstr.length() == len - 1);
        assert(!std::memcmp(intstr.data(), data, len * sizeof(int)));
        mem_handler.check(1, 0);

        std::printf("OK\n  copy, resize, swap... ");

        test2 = test;
        assert(test2.valid() && test2.length() == 3);
        assert(!std::memcmp(test2.data(), str, 3) && !test2.data()[3]);
        mem_handler.check(0, 1);

        dtest.resize(4);
        assert(dtest.valid() && dtest.length() == 3);
        std::memcpy(dtest.buffer(), str, 3);  dtest.buffer()[3] = '\0';
        assert(!std::memcmp(dtest.data(), str, 3) && !dtest.data()[3]);
        mem_handler.check(1, 1);

        test = str;
        assert(test.valid() && test.length() == len);
        assert(!std::memcmp(test.data(), str, len + 1));
        mem_handler.check(1, 0);

        test = "string";
        assert(test.valid() && test.length() == len);
        assert(!std::memcmp(test.data(), str, len + 1));
        mem_handler.check(1, 1);

        swap(test, test2);
        assert(test.valid() && test.length() == 3);
        assert(!std::memcmp(test.data(), str, 3) && !test.data()[3]);
        assert(test2.valid() && test2.length() == len);
        assert(!std::memcmp(test2.data(), str, len + 1));
        mem_handler.check(0, 0);

        std::printf("OK\n  destructors... ");
    }
    mem_handler.check(0, 6);

    std::printf("OK\n  concatenation, substr()... ");
    {
        NTL::Literal null, ltr("st~~~", 2);
        NTL::String inv1(null + ltr);
        assert(!inv1.valid() && !inv1.length() && !inv1.data());
        mem_handler.check(0, 0);

        NTL::String inv2(ltr + null);
        assert(!inv2.valid() && !inv2.length() && !inv2.data());
        mem_handler.check(0, 0);

        NTL::String zero(NTL::Literal("ze") + "\0ro");
        assert(zero.valid() && zero.length() == 5);
        assert(!std::memcmp(zero.data(), "ze\0ro", 6));
        mem_handler.check(1, 0);

        NTL::String test1(ltr + NTL::Literal("ring~~~", 4));
        assert(test1.valid() && test1.length() == len);
        assert(!std::memcmp(test1.data(), str, len + 1));
        mem_handler.check(1, 0);

        NTL::String test2(ltr + "ring");
        assert(test2.valid() && test2.length() == len);
        assert(!std::memcmp(test2.data(), str, len + 1));
        mem_handler.check(1, 0);

        NTL::String test3("str" + NTL::Literal("ing~~~", 3));
        assert(test3.valid() && test3.length() == len);
        assert(!std::memcmp(test3.data(), str, len + 1));
        mem_handler.check(1, 0);

        NTL::String test4(NTL::Literal("strin~~~", 5) + 'g');
        assert(test4.valid() && test4.length() == len);
        assert(!std::memcmp(test4.data(), str, len + 1));
        mem_handler.check(1, 0);

        NTL::String test5('s' + NTL::Literal("tring~~~", 5));
        assert(test5.valid() && test5.length() == len);
        assert(!std::memcmp(test5.data(), str, len + 1));
        mem_handler.check(1, 0);

        zero = "ze\0" + NTL::Literal("ro");
        assert(zero.valid() && zero.length() == 5);
        assert(!std::memcmp(zero.data(), "ze\0ro", 6));
        mem_handler.check(1, 1);

        test1 = inv1 + test1;
        assert(!test1.valid() && !test1.length() && !test1.data());
        mem_handler.check(0, 1);

        test2 = test2.substr(0, 4) + "ng";
        assert(test2.valid() && test2.length() == len);
        assert(!std::memcmp(test2.data(), str, len + 1));
        mem_handler.check(1, 1);

        test3 = "st" + test3.substr(2);
        assert(test3.valid() && test3.length() == len);
        assert(!std::memcmp(test3.data(), str, len + 1));
        mem_handler.check(1, 1);

        test4 = 's' + test4.substr(1);
        assert(test4.valid() && test4.length() == len);
        assert(!std::memcmp(test4.data(), str, len + 1));
        mem_handler.check(1, 1);

        test5 = test5.substr(0, 5) + 'g';
        assert(test5.valid() && test5.length() == len);
        assert(!std::memcmp(test5.data(), str, len + 1));
        mem_handler.check(1, 1);

        test1 = ltr;
        assert(test1.valid() && test1.length() == 2);
        assert(!std::memcmp(test1.data(), str, 2) && !test1.data()[2]);
        mem_handler.check(1, 0);

        test1 += NTL::Literal("rin~~~", 3);
        assert(test1.valid() && test1.length() == 5);
        assert(!std::memcmp(test1.data(), str, 5) && !test1.data()[5]);
        mem_handler.check(1, 1);

        test1 += 'g';
        assert(test1.valid() && test1.length() == len);
        assert(!std::memcmp(test1.data(), str, len + 1));
        mem_handler.check(1, 1);

        test1 = test2.substr(0, 2) + test1.substr(2, 2) + test3.substr(4);
        assert(test1.valid() && test1.length() == len);
        assert(!std::memcmp(test1.data(), str, len + 1));
        mem_handler.check(1, 1);

        std::printf("OK\n  operator[], at(), scan(), rscan()... ");

        for(size_t i = 0; i <= len; i++)
            assert(test1[i] == str[i] && test1.at(i) == str[i]);

        assert(test1.scan('i') == 3 && test1.rscan('t') == 1 && test1.scan('z') == size_t(-1));

        std::printf("OK\n  comparisons... ");

        assert(!test1.cmp(test2) && test1.cmp(ltr) > 0 && test1.cmp("sts") < 0);
        assert(test1 != test2.substr(3) && test1 < NTL::Literal("sts") && test1 > "str");

        std::printf("OK\n  memory problems... ");

        mem_handler.make_fail(1);
        test1 += test2;
        assert(!test1.valid() && !test1.length() && !test1.data());
        mem_handler.check(0, 1);

        test1 = test2 + test3 + test4;
        assert(!test1.valid() && !test1.length() && !test1.data());
        mem_handler.make_reliable();
        mem_handler.check(0, 0);
    }
    mem_handler.check(0, 5);

std::printf("OK\n");
}


void test_literal()
{
    test_literal_class();
    test_string_class();
}
