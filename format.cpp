/***************************************************************************************************
* Nothrow Template Library : format.cpp : test suite for format.h
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
#include "format.h"
#include <cstdio>



void test_arg_func()
{
    const char *str = "string";  size_t len = std::strlen(str);
    std::printf("\nTesting arg() functions:\n  basic... ");

    mem_handler.count = 0;
    mem_handler.make_reliable();
    {
        NTL::String test(NTL::arg<'A'>(NTL::Literal(str, 2) + NTL::Literal(str + 2, len - 2)));
        assert(!std::memcmp(test.data(), str, len + 1));
        assert(mem_handler.count == 1);

        test = NTL::arg<'B'>(str, 3);
        assert(!std::memcmp(test.data(), str, 3) && !test.data()[3]);
        assert(mem_handler.count == 1);

        test = NTL::arg<'C'>(str);
        assert(!std::memcmp(test.data(), str, len + 1));
        assert(mem_handler.count == 1);

        test = NTL::arg(test.substr(0, 4) + NTL::Literal(str + 4, len - 4));
        assert(!std::memcmp(test.data(), str, len + 1));
        assert(mem_handler.count == 1);

        test = NTL::arg(str, 4);
        assert(!std::memcmp(test.data(), str, 4) && !test.data()[4]);
        assert(mem_handler.count == 1);

        test = NTL::arg(str);
        assert(!std::memcmp(test.data(), str, len + 1));
        assert(mem_handler.count == 1);

        // TODO: numeric args
    }
    assert(!mem_handler.count);
    mem_handler.make_fail(1);

    std::printf("OK\n");
}


void test_format_func()
{
    std::printf("\nTesting format() functions:\n  without arguments... ");

    mem_handler.count = 0;
    mem_handler.make_reliable();
    {
        NTL::String test(NTL::format<'#'>(NTL::Literal("A#3B#2C#1D###")));
        assert(!std::strcmp(test.data(), "A3B2C1D#"));
        assert(mem_handler.count == 1);

        test = NTL::format<'$'>("A$3B$2C$1D$$$~~~", 13);
        assert(!std::strcmp(test.data(), "A3B2C1D$"));
        assert(mem_handler.count == 1);

        test = NTL::format<'@'>("A@3B@2C@1D@@@");
        assert(!std::strcmp(test.data(), "A3B2C1D@"));
        assert(mem_handler.count == 1);

        test = NTL::format(NTL::Literal("A%3B%2C%1D%%%"));
        assert(!std::strcmp(test.data(), "A3B2C1D%"));
        assert(mem_handler.count == 1);

        test = NTL::format("A%3B%2C%1D%%%~~~", 13);
        assert(!std::strcmp(test.data(), "A3B2C1D%"));
        assert(mem_handler.count == 1);

        test = NTL::format("A%3B%2C%1D%%%");
        assert(!std::strcmp(test.data(), "A3B2C1D%"));
        assert(mem_handler.count == 1);

        std::printf("OK\n  operator %%... ");

        test = NTL::format(" %3%%%4 %A%2%A %") % NTL::arg<'A'>("-A-") % NTL::arg("*2*~", 3) % NTL::Literal("-3-") % "-4-";
        assert(!std::strcmp(test.data(), " -3-%-4- -A-*2*-A- "));
        assert(mem_handler.count == 1);

        test = NTL::format<'$'>("_$4$$$3_$1$B$1_$") % NTL::arg("*1*~", 3) % NTL::arg<'B'>("-B-") % "-3-" % NTL::Literal("-4-");
        assert(!std::strcmp(test.data(), "_-4-$-3-_*1*-B-*1*_"));
        assert(mem_handler.count == 1);

        test = NTL::format("%%-%3-%3-%2-%%%") % "Aa" % "Bb" % "Cc";
        assert(!std::strcmp(test.data(), "%-Cc-Cc-Bb-%"));
        assert(mem_handler.count == 1);

        std::printf("OK\n  switch optimization... ");

        test = NTL::format<'#'>("###a#") % NTL::arg<'a'>("A");
        assert(!std::strcmp(test.data(), "#A"));
        assert(mem_handler.count == 1);

        test = NTL::format<'#'>("###a#b#") % NTL::arg<'a'>("A") % NTL::arg<'b'>("B");
        assert(!std::strcmp(test.data(), "#AB"));
        assert(mem_handler.count == 1);

        test = NTL::format<'#'>("###a#b#c#") % NTL::arg<'a'>("A") % NTL::arg<'b'>("B") % NTL::arg<'c'>("C");
        assert(!std::strcmp(test.data(), "#ABC"));
        assert(mem_handler.count == 1);

        test = NTL::format("%%%1%2%3%4%") % "A" % "B" % "C" % "D";
        assert(!std::strcmp(test.data(), "%ABCD"));
        assert(mem_handler.count == 1);

        test = NTL::format("%%%1%2%3%4%5%") % "A" % "B" % "C" % "D" % "E";
        assert(!std::strcmp(test.data(), "%ABCDE"));
        assert(mem_handler.count == 1);

        test = NTL::format("%%%1%2%3%4%5%6%") % "A" % "B" % "C" % "D" % "E" % "F";
        assert(!std::strcmp(test.data(), "%ABCDEF"));
        assert(mem_handler.count == 1);

        test = NTL::format("%%%1%2%3%4%5%6%7%") % "A" % "B" % "C" % "D" % "E" % "F" % "G";
        assert(!std::strcmp(test.data(), "%ABCDEFG"));
        assert(mem_handler.count == 1);

        std::printf("OK\n  string-like properties... ");

        test = "---" + NTL::format("%3%2~%2%1") % "*" % "|" % "*" + NTL::Literal("---", 2) + '-';
        assert(!std::strcmp(test.data(), "---*|~|*---"));
        assert(mem_handler.count == 1);
    }
    assert(!mem_handler.count);
    mem_handler.make_fail(1);

    std::printf("OK\n");
}


void test_format()
{
    test_arg_func();
    test_format_func();
}
