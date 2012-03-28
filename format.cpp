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
#include "fmt-int.h"
#include <cstdio>




template<typename T, typename Tf> void test_dec(T num, const char *fmt)
{
    char buf[256];
    std::sprintf(buf, fmt, Tf(num));
    size_t len = std::strlen(buf);
    {
        NTL::String test = NTL::dec(num);
        assert(test.length() == len && !std::memcmp(test.data(), buf, len + 1));
        mem_handler.check(1, 0);

        test = NTL::dec<'Z'>(num);
        assert(test.length() == len && !std::memcmp(test.data(), buf, len + 1));
        mem_handler.check(1, 1);
    }
    mem_handler.check(0, 1);
}

template<typename T> void test_udec(T num)
{
    test_dec<T, unsigned long>(num, "%lu");
}

template<typename T, typename Tf> void test_sdec(T num,
    const char *fmt, const char *fmtp, const char *fmts)
{
    char buf[256];  size_t len;
    {
        std::sprintf(buf, fmt, Tf(num));  len = std::strlen(buf);

        NTL::String test = NTL::dec(num);
        assert(test.length() == len && !std::memcmp(test.data(), buf, len + 1));
        mem_handler.check(1, 0);

        if(*buf == '-')*buf = '=';

        test = NTL::dec<'Z'>(num, "=");
        assert(test.length() == len && !std::memcmp(test.data(), buf, len + 1));
        mem_handler.check(1, 1);

        std::sprintf(buf, fmtp, Tf(num));  len = std::strlen(buf);

        test = NTL::dec(num, "-", "+");
        assert(test.length() == len && !std::memcmp(test.data(), buf, len + 1));
        mem_handler.check(1, 1);

        std::sprintf(buf, fmts, Tf(num));  len = std::strlen(buf);
        if(*buf == '-')*buf = '_';

        test = NTL::dec<'8'>(num, "_", " ");
        assert(test.length() == len && !std::memcmp(test.data(), buf, len + 1));
        mem_handler.check(1, 1);

        std::sprintf(buf + 2, fmtp, Tf(num));
        buf[0] = buf[1] = buf[2];  len = std::strlen(buf);

        NTL::String test1 = NTL::dec<'*'>(num, "---", "+++");
        assert(test1.length() == len && !std::memcmp(test1.data(), buf, len + 1));
        mem_handler.check(1, 0);
    }
    mem_handler.check(0, 2);
}

template<typename T> void test_sdec(T num)
{
    test_sdec<T, long>(num, "%ld", "%+ld", "% ld");
}


void test_arg_func()
{
    const char *str = "string";  size_t len = std::strlen(str);
    std::printf("\nTesting argument functions:\n  arg()... ");

    mem_handler.reset();
    {
        NTL::String test(NTL::arg<'A'>(NTL::Literal(str, 2) + NTL::Literal(str + 2, len - 2)));
        assert(!std::memcmp(test.data(), str, len + 1));
        mem_handler.check(1, 0);

        test = NTL::arg<'B'>(str, 3);
        assert(!std::memcmp(test.data(), str, 3) && !test.data()[3]);
        mem_handler.check(1, 1);

        test = NTL::arg<'C'>(str);
        assert(!std::memcmp(test.data(), str, len + 1));
        mem_handler.check(1, 1);

        test = NTL::arg(test.substr(0, 4) + NTL::Literal(str + 4, len - 4));
        assert(!std::memcmp(test.data(), str, len + 1));
        mem_handler.check(1, 1);

        test = NTL::arg(str, 4);
        assert(!std::memcmp(test.data(), str, 4) && !test.data()[4]);
        mem_handler.check(1, 1);

        test = NTL::arg(str);
        assert(!std::memcmp(test.data(), str, len + 1));
        mem_handler.check(1, 1);

        std::printf("OK\n  unsigned dec()... ");

        test_udec<unsigned char>(0);
        test_udec<unsigned char>(123);
        test_udec<unsigned char>(UCHAR_MAX);

        test_udec<unsigned short>(0);
        test_udec<unsigned short>(54321);
        test_udec<unsigned short>(USHRT_MAX);

        test_udec<unsigned>(0);
        test_udec<unsigned>(54321);
        test_udec<unsigned>(UINT_MAX);

        test_udec<unsigned long>(0);
        test_udec<unsigned long>(987654321);
        test_udec<unsigned long>(ULONG_MAX);

#ifdef ULLONG_MAX
        test_dec<unsigned long long, unsigned long long>(0, "%llu");
        test_dec<unsigned long long, unsigned long long>(9876543210987654321llu, "%llu");
        test_dec<unsigned long long, unsigned long long>(ULLONG_MAX, "%llu");
#endif

        std::printf("OK\n  signed dec()... ");

        test_dec<char, int>(CHAR_MIN, "%d");
        test_dec<char, int>(123, "%d");
        test_dec<char, int>(CHAR_MAX, "%d");

        test_sdec<signed char>(SCHAR_MIN);
        test_sdec<signed char>(-123);
        test_sdec<signed char>(123);
        test_sdec<signed char>(SCHAR_MAX);

        test_sdec<signed short>(SHRT_MIN);
        test_sdec<signed short>(-4321);
        test_sdec<signed short>(4321);
        test_sdec<signed short>(SHRT_MAX);

        test_sdec<int>(INT_MIN);
        test_sdec<int>(-4321);
        test_sdec<int>(4321);
        test_sdec<int>(INT_MAX);

        test_sdec<signed long>(LONG_MIN);
        test_sdec<signed long>(-987654321);
        test_sdec<signed long>(987654321);
        test_sdec<signed long>(LONG_MAX);

#ifdef ULLONG_MAX
        test_sdec<long long, long long>(LLONG_MIN, "%lld", "%+lld", "% lld");
        test_sdec<long long, long long>(-987654321987654321ll, "%lld", "%+lld", "% lld");
        test_sdec<long long, long long>(987654321987654321ll, "%lld", "%+lld", "% lld");
        test_sdec<long long, long long>(LLONG_MAX, "%lld", "%+lld", "% lld");
#endif
    }
    mem_handler.check(0, 1);

    std::printf("OK\n");
}


void test_format_func()
{
    std::printf("\nTesting format() functions:\n  without arguments... ");

    mem_handler.reset();
    {
        NTL::String test(NTL::format<'#'>(NTL::Literal("A#3B#2C#1D###")));
        assert(!std::strcmp(test.data(), "A3B2C1D#"));
        mem_handler.check(1, 0);

        test = NTL::format<'$'>("A$3B$2C$1D$$$~~~", 13);
        assert(!std::strcmp(test.data(), "A3B2C1D$"));
        mem_handler.check(1, 1);

        test = NTL::format<'@'>("A@3B@2C@1D@@@");
        assert(!std::strcmp(test.data(), "A3B2C1D@"));
        mem_handler.check(1, 1);

        test = NTL::format(NTL::Literal("A%3B%2C%1D%%%"));
        assert(!std::strcmp(test.data(), "A3B2C1D%"));
        mem_handler.check(1, 1);

        test = NTL::format("A%3B%2C%1D%%%~~~", 13);
        assert(!std::strcmp(test.data(), "A3B2C1D%"));
        mem_handler.check(1, 1);

        test = NTL::format("A%3B%2C%1D%%%");
        assert(!std::strcmp(test.data(), "A3B2C1D%"));
        mem_handler.check(1, 1);

        std::printf("OK\n  operator %%... ");

        test = NTL::format(" %3%%%4 %A%2%A %") % NTL::arg<'A'>("-A-") % NTL::arg("*2*~", 3) % NTL::Literal("-3-") % "-4-";
        assert(!std::strcmp(test.data(), " -3-%-4- -A-*2*-A- "));
        mem_handler.check(1, 1);

        test = NTL::format<'$'>("_$4$$$3_$1$B$1_$") % NTL::arg("*1*~", 3) % NTL::arg<'B'>("-B-") % "-3-" % NTL::Literal("-4-");
        assert(!std::strcmp(test.data(), "_-4-$-3-_*1*-B-*1*_"));
        mem_handler.check(1, 1);

        test = NTL::format("%%-%3-%3-%2-%%%") % "Aa" % "Bb" % "Cc";
        assert(!std::strcmp(test.data(), "%-Cc-Cc-Bb-%"));
        mem_handler.check(1, 1);

        std::printf("OK\n  switch optimization... ");

        test = NTL::format<'#'>("###a#") % NTL::arg<'a'>("A");
        assert(!std::strcmp(test.data(), "#A"));
        mem_handler.check(1, 1);

        test = NTL::format<'#'>("###a#b#") % NTL::arg<'a'>("A") % NTL::arg<'b'>("B");
        assert(!std::strcmp(test.data(), "#AB"));
        mem_handler.check(1, 1);

        test = NTL::format<'#'>("###a#b#c#") % NTL::arg<'a'>("A") % NTL::arg<'b'>("B") % NTL::arg<'c'>("C");
        assert(!std::strcmp(test.data(), "#ABC"));
        mem_handler.check(1, 1);

        test = NTL::format("%%%1%2%3%4%") % "A" % "B" % "C" % "D";
        assert(!std::strcmp(test.data(), "%ABCD"));
        mem_handler.check(1, 1);

        test = NTL::format("%%%1%2%3%4%5%") % "A" % "B" % "C" % "D" % "E";
        assert(!std::strcmp(test.data(), "%ABCDE"));
        mem_handler.check(1, 1);

        test = NTL::format("%%%1%2%3%4%5%6%") % "A" % "B" % "C" % "D" % "E" % "F";
        assert(!std::strcmp(test.data(), "%ABCDEF"));
        mem_handler.check(1, 1);

        test = NTL::format("%%%1%2%3%4%5%6%7%") % "A" % "B" % "C" % "D" % "E" % "F" % "G";
        assert(!std::strcmp(test.data(), "%ABCDEFG"));
        mem_handler.check(1, 1);

        std::printf("OK\n  string-like properties... ");

        test = "---" + NTL::format("%3%2~%2%1") % "*" % "|" % "*" + NTL::Literal("---", 2) + '-';
        assert(!std::strcmp(test.data(), "---*|~|*---"));
        mem_handler.check(1, 1);
    }
    mem_handler.check(0, 1);

    std::printf("OK\n");
}


void test_format()
{
    test_arg_func();
    test_format_func();
}
