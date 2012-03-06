/***************************************************************************************************
* Nothrow Template Library : win-utf.cpp : test suite for win-utf.h
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
#include "win-utf.h"
#include <cstdio>



void test_wide()
{
    std::printf("\nTesting UTF8<->UTF16 infrastructure:\n  wide strlen/cmp... ");

    NTL::WLiteral str = L"string";
    assert(str.length() == 6);

    NTL::WLiteral part = str.substr(0, 3);
    assert(NTL::WLiteral(L"").cmp(NTL::WLiteral()) > 0);
    assert(!part.cmp(L"str") && part.cmp(L"sts") < 0 && part.cmp(L"stq") > 0);
    assert(part.cmp(NTL::WLiteral(L"str", 4)) < 0 && part.cmp(L"stq ") > 0);
    assert(NTL::WLiteral(L"str", 4).cmp(part) > 0 && part.cmp(L"su") < 0);
    assert(part == L"str" && part < NTL::WLiteral(L"str", 4) && part > L"r");
    assert(part <= L"str" && part >= L"str" && !(part != L"str"));

    std::printf("OK\n  utf8to16(), utf16to8()... ");

    mem_handler.count = 0;
    mem_handler.make_reliable();
    {
        typedef NTL::StringBase<wchar_t> WStr;
        WStr test1 = NTL::utf8to16(NTL::Literal("test") + ' ') + NTL::utf8to16("string~~~", 6) + NTL::utf8to16(" here");
        assert(!std::wcscmp(test1.data(), L"test string here"));
        assert(mem_handler.count == 1);

        NTL::String test2 = NTL::utf16to8(NTL::WLiteral(L"test") + L' ') + NTL::utf16to8(L"string~~~", 6) + NTL::utf16to8(L" here");
        assert(!std::strcmp(test2.data(), "test string here"));
        assert(mem_handler.count == 2);

        //std::printf("OK\n  conversion correctness... ");

        // TODO
    }
    assert(!mem_handler.count);
    mem_handler.make_fail(1);

    std::printf("OK\n");
}


void test_wstring_class()
{
    std::printf("\nTesting WString class:\n  constructors... ");

    mem_handler.count = 0;
    {
        NTL::WString null;
        assert(!null.valid() && !null.length() && !null.data());
        assert(!mem_handler.count);

        mem_handler.make_reliable();
        NTL::WString test = NTL::WLiteral(L"str~~~", 3) + L"ing";
        assert(!std::wcscmp(test.data(), L"string"));
        assert(mem_handler.count == 1);

        NTL::WString test1 = L"string";
        assert(!std::wcscmp(test1.data(), L"string"));
        assert(mem_handler.count == 2);

        NTL::WString test2(L"str~~~", 3);
        assert(!std::wcscmp(test2.data(), L"str"));
        assert(mem_handler.count == 3);

        mem_handler.make_fail(1);
        NTL::WString copy1 = test1;
        assert(!std::wcscmp(copy1.data(), L"string"));
        assert(mem_handler.count == 3);

        NTL::WString copy2 = static_cast<NTL::StringBase<wchar_t> &>(test2);
        assert(!std::wcscmp(copy2.data(), L"str"));
        assert(mem_handler.count == 3);

        mem_handler.make_reliable();
        NTL::WString test3 = NTL::Literal("str~~~", 3) + "ing";
        assert(!std::wcscmp(test3.data(), L"string"));
        assert(mem_handler.count == 4);

        NTL::WString test4 = "string";
        assert(!std::wcscmp(test4.data(), L"string"));
        assert(mem_handler.count == 5);

        NTL::WString test5("str~~~", 3);
        assert(!std::wcscmp(test5.data(), L"str"));
        assert(mem_handler.count == 6);

        std::printf("OK\n  copy, concatenation, swap... ");

        test = test1.substr(0, 3) + L"ing";
        assert(!std::wcscmp(test.data(), L"string"));
        assert(mem_handler.count == 6);

        test = L"str";
        assert(!std::wcscmp(test.data(), L"str"));
        assert(mem_handler.count == 6);

        test = NTL::Literal("str~~~", 3) + "ing";
        assert(!std::wcscmp(test.data(), L"string"));
        assert(mem_handler.count == 6);

        test = "str";
        assert(!std::wcscmp(test.data(), L"str"));
        assert(mem_handler.count == 6);

        mem_handler.make_fail(1);  test3 = test;
        assert(!std::wcscmp(test3.data(), L"str"));
        assert(mem_handler.count == 5);

        mem_handler.make_reliable();
        test4 = static_cast<NTL::StringBase<wchar_t> &>(test);
        assert(!std::wcscmp(test4.data(), L"str"));
        assert(mem_handler.count == 4);

        test += test1.substr(3, 1) + L"n";
        assert(!std::wcscmp(test.data(), L"strin"));
        assert(mem_handler.count == 5);

        test += L"g";
        assert(!std::wcscmp(test.data(), L"string"));
        assert(mem_handler.count == 5);

        test += NTL::Literal(".") + ".";
        assert(!std::wcscmp(test.data(), L"string.."));
        assert(mem_handler.count == 5);

        test += "..";
        assert(!std::wcscmp(test.data(), L"string...."));
        assert(mem_handler.count == 5);

        mem_handler.make_fail(1);  NTL::swap(test1, test5);
        assert(!std::wcscmp(test1.data(), L"str"));
        assert(!std::wcscmp(test5.data(), L"string"));
        assert(mem_handler.count == 5);

        std::printf("OK\n  back conversion... ");

        mem_handler.make_reliable();
        NTL::String str = "...." + test.utf8();
        assert(!std::strcmp(str.data(), "....string...."));
        assert(mem_handler.count == 6);
    }
    assert(!mem_handler.count);

    std::printf("OK\n");
}


void test_win_utf()
{
    test_wide();
    test_wstring_class();
}
