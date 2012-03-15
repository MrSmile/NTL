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



const char utf8_test[] =
// Greek word 'kosme':
    "\xCE\xBA\xE1\xBD\xB9\xCF\x83\xCE\xBC\xCE\xB5 "
// First possible sequence of a certain length:
    "\x00 \xC2\x80 \xE0\xA0\x80 \xF0\x90\x80\x80 \xF8\x88\x80\x80\x80 \xFC\x84\x80\x80\x80\x80 "
// Last possible sequence of a certain length:
    "\x7F \xDF\xBF \xEF\xBF\xBF \xF7\xBF\xBF\xBF \xFB\xBF\xBF\xBF\xBF \xFD\xBF\xBF\xBF\xBF\xBF "
// Other boundary conditions:
    "\xED\x9F\xBF \xEE\x80\x80 \xEF\xBF\xBD \xF4\x8F\xBF\xBF \xF4\x90\x80\x80 "
// Unexpected continuation bytes:
    "\x80 \xBF \x80\xBF \x80\xBF\x80 \x80\xBF\x80\xBF \x80\xBF\x80\xBF\x80 \x80\xBF\x80\xBF\x80\xBF \x80\xBF\x80\xBF\x80\xBF\x80 "
// Sequence of all 64 possible continuation bytes (0x80-0xBF):
    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F "
    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F "
    "\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF "
    "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF "
// All 32 first bytes of 2-byte sequences (0xC0-0xDF):
    "\xC0 \xC1 \xC2 \xC3 \xC4 \xC5 \xC6 \xC7 \xC8 \xC9 \xCA \xCB \xCC \xCD \xCE \xCF "
    "\xD0 \xD1 \xD2 \xD3 \xD4 \xD5 \xD6 \xD7 \xD8 \xD9 \xDA \xDB \xDC \xDD \xDE \xDF "
// All 16 first bytes of 3-byte sequences (0xE0-0xEF):
    "\xE0 \xE1 \xE2 \xE3 \xE4 \xE5 \xE6 \xE7 \xE8 \xE9 \xEA \xEB \xEC \xED \xEE \xEF "
// All 8 first bytes of 4-byte sequences (0xF0-0xF7):
// All 4 first bytes of 5-byte sequences (0xF8-0xFB):
// All 2 first bytes of 6-byte sequences (0xFC-0xFD):
    "\xF0 \xF1 \xF2 \xF3 \xF4 \xF5 \xF6 \xF7 \xF8 \xF9 \xFA \xFB \xFC \xFD "
// Sequences with last continuation byte missing:
    "\xC0 \xE0\x80 \xF0\x80\x80 \xF8\x80\x80\x80 \xFC\x80\x80\x80\x80 "
    "\xDF \xEF\xBF \xF7\xBF\xBF \xFB\xBF\xBF\xBF \xFD\xBF\xBF\xBF\xBF "
// Concatenation of 10 previous incomplete sequences:
    "\xC0\xE0\x80\xF0\x80\x80\xF8\x80\x80\x80\xFC\x80\x80\x80\x80\xDF\xEF\xBF\xF7\xBF\xBF\xFB\xBF\xBF\xBF\xFD\xBF\xBF\xBF\xBF "
// Impossible bytes:
    "\xFE \xFF \xFE\xFE\xFF\xFF "
// Overlong sequences:
    "\xC0\xAF \xE0\x80\xAF \xF0\x80\x80\xAF \xF8\x80\x80\x80\xAF \xFC\x80\x80\x80\x80\xAF "
// Maximum overlong sequences:
    "\xC1\xBF \xE0\x9F\xBF \xF0\x8F\xBF\xBF \xF8\x87\xBF\xBF\xBF \xFC\x83\xBF\xBF\xBF\xBF "
// Overlong representation of the NUL character:
    "\xC0\x80 \xE0\x80\x80 \xF0\x80\x80\x80 \xF8\x80\x80\x80\x80 \xFC\x80\x80\x80\x80\x80 "
// Single UTF-16 surrogates:
    "\xED\xA0\x80 \xED\xAD\xBF \xED\xAE\x80 \xED\xAF\xBF \xED\xB0\x80 \xED\xBE\x80 \xED\xBF\xBF "
// Paired UTF-16 surrogates:
    "\xED\xA0\x80\xED\xB0\x80 \xED\xA0\x80\xED\xBF\xBF \xED\xAD\xBF\xED\xB0\x80 \xED\xAD\xBF\xED\xBF\xBF "
    "\xED\xAE\x80\xED\xB0\x80 \xED\xAE\x80\xED\xBF\xBF \xED\xAF\xBF\xED\xB0\x80 \xED\xAF\xBF\xED\xBF\xBF "
// Other illegal code positions:
    "\xEF\xBF\xBE \xEF\xBF\xBF ";

const wchar_t utf8_at16[] =
// Greek word 'kosme':
    L"\x03BA\x1F79\x03C3\x03BC\x03B5 "
// First possible sequence of a certain length:
    L"\x0000 \x0080 \x0800 \xD800\xDC00 \xFFFD \xFFFD "
// Last possible sequence of a certain length:
    L"\x007F \x07FF \xFFFF \xFFFD \xFFFD \xFFFD "
// Other boundary conditions:
    L"\xD7FF \xE000 \xFFFD \xDBFF\xDFFF \xFFFD "
// Unexpected continuation bytes:
    L"\xFFFD \xFFFD \xFFFD\xFFFD \xFFFD\xFFFD\xFFFD \xFFFD\xFFFD\xFFFD\xFFFD \xFFFD\xFFFD\xFFFD\xFFFD\xFFFD "
    L"\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD \xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD "
// Sequence of all 64 possible continuation bytes (0x80-0xBF):
    L"\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD "
    L"\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD "
    L"\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD "
    L"\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD "
// All 32 first bytes of 2-byte sequences (0xC0-0xDF):
    L"\xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD "
    L"\xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD "
// All 16 first bytes of 3-byte sequences (0xE0-0xEF):
    L"\xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD "
// All 8 first bytes of 4-byte sequences (0xF0-0xF7):
// All 4 first bytes of 5-byte sequences (0xF8-0xFB):
// All 2 first bytes of 6-byte sequences (0xFC-0xFD):
    L"\xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD "
// Sequences with last continuation byte missing:
    L"\xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD "
// Concatenation of 10 previous incomplete sequences:
    L"\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD "
// Impossible bytes:
    L"\xFFFD \xFFFD \xFFFD\xFFFD\xFFFD\xFFFD "
// Overlong sequences:
    L"\xFFFD \xFFFD \xFFFD \xFFFD \xFFFD "
// Maximum overlong sequences:
    L"\xFFFD \xFFFD \xFFFD \xFFFD \xFFFD "
// Overlong representation of the NUL character:
    L"\xFFFD \xFFFD \xFFFD \xFFFD \xFFFD "
// Single UTF-16 surrogates:
    L"\xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD \xFFFD "
// Paired UTF-16 surrogates:
    L"\xFFFD\xFFFD \xFFFD\xFFFD \xFFFD\xFFFD \xFFFD\xFFFD \xFFFD\xFFFD \xFFFD\xFFFD \xFFFD\xFFFD \xFFFD\xFFFD "
// Other illegal code positions:
    L"\xFFFE \xFFFF ";


const wchar_t utf16_test[] =
// Greek word 'kosme':
    L"\x03BA\x1F79\x03C3\x03BC\x03B5 "
// First possible sequence of a certain length:
    L"\x0000 \x0080 \x0800 \xD800\xDC00 "
// Last possible sequence of a certain length:
    L"\x007F \x07FF \xFFFF "
// Other boundary conditions:
    L"\xD7FF \xE000 \xFFFD \xDBFF\xDFFF "
// Unexpected continuation surrogates:
    L"\xDC00 \xDD1F \xDE50 \xDF80 \xDFFF \xDC00\xDFFF "
// First surrogates:
    L"\xD800 \xD91F \xDA50 \xDB80 \xDBFF "
// Concatenation of previous incomplete sequences:
    L"\xD800\xD91F\xDA50\xDB80\xDBFF "
// Paired UTF-16 surrogates:
    L"\xD800\xDC00 \xD800\xDFFF \xDB7F\xDC00 \xDB7F\xDFFF \xDB80\xDC00 \xDB80\xDFFF \xDBFF\xDC00 \xDBFF\xDFFF "
// Other illegal code positions:
    L"\xFFFE \xFFFF ";

const char utf16_at8[] =
// Greek word 'kosme':
    "\xCE\xBA\xE1\xBD\xB9\xCF\x83\xCE\xBC\xCE\xB5 "
// First possible sequence of a certain length:
    "\x00 \xC2\x80 \xE0\xA0\x80 \xF0\x90\x80\x80 "
// Last possible sequence of a certain length:
    "\x7F \xDF\xBF \xEF\xBF\xBF "
// Other boundary conditions:
    "\xED\x9F\xBF \xEE\x80\x80 \xEF\xBF\xBD \xF4\x8F\xBF\xBF "
// Unexpected continuation surrogates:
    "\xEF\xBF\xBD \xEF\xBF\xBD \xEF\xBF\xBD \xEF\xBF\xBD \xEF\xBF\xBD \xEF\xBF\xBD\xEF\xBF\xBD "
// First surrogates:
    "\xEF\xBF\xBD \xEF\xBF\xBD \xEF\xBF\xBD \xEF\xBF\xBD \xEF\xBF\xBD "
// Concatenation of previous incomplete sequences:
    "\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD "
// Paired UTF-16 surrogates:
    "\xF0\x90\x80\x80 \xF0\x90\x8F\xBF \xF3\xAF\xB0\x80 \xF3\xAF\xBF\xBF "
    "\xF3\xB0\x80\x80 \xF3\xB0\x8F\xBF \xF4\x8F\xB0\x80 \xF4\x8F\xBF\xBF "
// Other illegal code positions:
    "\xEF\xBF\xBE \xEF\xBF\xBF ";




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

        std::printf("OK\n  conversion correctness... ");

        size_t len = sizeof(utf8_at16) / sizeof(wchar_t);
        test1 = NTL::utf8to16(utf8_test, sizeof(utf8_test) - 1);
        assert(test1.length() == len - 1 && !std::memcmp(test1.data(), utf8_at16, len * sizeof(wchar_t)));
        assert(mem_handler.count == 2);

        len = sizeof(utf16_at8);
        test2 = NTL::utf16to8(utf16_test, sizeof(utf16_test) / sizeof(wchar_t) - 1);
        assert(test2.length() == len - 1 && !std::memcmp(test2.data(), utf16_at8, len));
        assert(mem_handler.count == 2);
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
        NTL::WString test(NTL::WLiteral(L"str~~~", 3) + L"ing");
        assert(!std::wcscmp(test.data(), L"string"));
        assert(mem_handler.count == 1);

        NTL::WString test1(L"string");
        assert(!std::wcscmp(test1.data(), L"string"));
        assert(mem_handler.count == 2);

        NTL::WString test2(L"str~~~", 3);
        assert(!std::wcscmp(test2.data(), L"str"));
        assert(mem_handler.count == 3);

        mem_handler.make_fail(1);
        NTL::WString copy1(test1);
        assert(!std::wcscmp(copy1.data(), L"string"));
        assert(mem_handler.count == 3);

        NTL::WString copy2(static_cast<NTL::StringBase<wchar_t> &>(test2));
        assert(!std::wcscmp(copy2.data(), L"str"));
        assert(mem_handler.count == 3);

        mem_handler.make_reliable();
        NTL::WString test3(NTL::Literal("str~~~", 3) + "ing");
        assert(!std::wcscmp(test3.data(), L"string"));
        assert(mem_handler.count == 4);

        NTL::WString test4("string");
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

        mem_handler.make_fail(1);  swap(test1, test5);
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
    mem_handler.make_fail(1);

    std::printf("OK\n");
}


void test_win_utf()
{
    test_wide();
    test_wstring_class();
}
