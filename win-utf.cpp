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



void test_win_utf()
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
