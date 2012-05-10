/***************************************************************************************************
* Nothrow Template Library : win-utf.h : utf8-utf16 conversion for Windows
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

#pragma once

#include "literal.h"
#include <cwchar>



namespace NTL_ {



template<> inline LiteralBase<wchar_t>::LiteralBase(const wchar_t *str) :
    ptr_(str), len_(str ? std::wcslen(str) : 0)
{
}

template<> inline int LiteralBase<wchar_t>::cmp(const wchar_t *str, size_t n) const
{
    if(!ptr_)return str ? -1 : 0;  if(!str)return 1;
    if(len_ < n)return std::wmemcmp(ptr_, str, len_) > 0 ? 1 : -1;
    if(len_ > n)return std::wmemcmp(ptr_, str, n) < 0 ? -1 : 1;
    return std::wmemcmp(ptr_, str, len_);
}

typedef LiteralBase<wchar_t> WLiteral;



typedef unsigned long uchar;


struct Utf8to16Counter
{
    size_t length;
    uchar next, lim;
    unsigned count;

    Utf8to16Counter() : length(0), count(0)
    {
    }

    bool operator () (const char *ptr, size_t len)
    {
        for(; len > 0; len--)operator () (*ptr++);  return true;
    }

    bool operator () (char ch)
    {
        if(!(ch & 0x80))
        {
            length += count ? 2 : 1;  count = 0;
        }
        else if(!(ch & 0x40))
        {
            if(count)
            {
                next = next << 6 | ch & 0x3F;
                if(!--count)length += next >= 0x10000 && next < 0x110000 ? 2 : 1;
            }
            else length++;
        }
        else
        {
            if(count)length++;
            if(!(ch & 0x20))
            {
                count = 1;  next = ch & 0x1F;  lim = 0x80;
            }
            else if(!(ch & 0x10))
            {
                count = 2;  next = ch & 0x0F;  lim = 0x800;
            }
            else if(!(ch & 0x08))
            {
                count = 3;  next = ch & 0x07;  lim = 0x10000;
            }
            else if(!(ch & 0x04))
            {
                count = 4;  next = ch & 0x03;  lim = 0x200000;
            }
            else if(!(ch & 0x02))
            {
                count = 5;  next = ch & 0x01;  lim = 0x4000000;
            }
            else
            {
                length++;  count = 0;
            }
        }
        return true;
    }

    size_t end()
    {
        return count ? length + 1 : length;
    }
};


template<typename B> struct Utf8to16Filler
{
    B &buf;
    uchar next, lim;
    unsigned count;

    Utf8to16Filler(B &ref) : buf(ref), count(0)
    {
    }

    bool operator () (const char *ptr, size_t len)
    {
        for(; len > 0; len--)if(!operator () (*ptr++))return false;  return true;
    }

    bool operator () (char ch)
    {
        if(!(ch & 0x80))
        {
            if(count && !buf(wchar_t(0xFFFD)))return false;
            count = 0;  if(!buf(wchar_t(ch)))return false;
        }
        else if(!(ch & 0x40))
        {
            if(count)
            {
                next = next << 6 | ch & 0x3F;
                if(!--count)
                {
                    if(next < lim)next = 0xFFFD;
                    else if(next < 0xE000)
                    {
                        if(next >= 0xD800)next = 0xFFFD;
                    }
                    else if(next >= 0x10000)
                    {
                        if(next < 0x110000)
                        {
                            next -= 0x10000;
                            if(!buf(wchar_t(0xD800 | next >> 10)))return false;
                            next = 0xDC00 | next & 0x3FF;
                        }
                        else next = 0xFFFD;
                    }
                    if(!buf(wchar_t(next)))return false;
                }
            }
            else if(!buf(wchar_t(0xFFFD)))return false;
        }
        else
        {
            if(count && !buf(wchar_t(0xFFFD)))return false;
            if(!(ch & 0x20))
            {
                count = 1;  next = ch & 0x1F;  lim = 0x80;
            }
            else if(!(ch & 0x10))
            {
                count = 2;  next = ch & 0x0F;  lim = 0x800;
            }
            else if(!(ch & 0x08))
            {
                count = 3;  next = ch & 0x07;  lim = 0x10000;
            }
            else if(!(ch & 0x04))
            {
                count = 4;  next = ch & 0x03;  lim = 0x200000;
            }
            else if(!(ch & 0x02))
            {
                count = 5;  next = ch & 0x01;  lim = 0x4000000;
            }
            else
            {
                if(!buf(L'\xFFFD'))return false;  count = 0;
            }
        }
        return true;
    }

    bool end()
    {
        return !count || buf(wchar_t(0xFFFD));
    }
};


struct Utf16to8Counter
{
    size_t length;
    bool next;

    Utf16to8Counter() : length(0), next(false)
    {
    }

    bool operator () (const wchar_t *ptr, size_t len)
    {
        for(; len > 0; len--)operator () (*ptr++);  return true;
    }

    bool operator () (wchar_t ch)
    {
        if(next)
        {
            next = false;
            if(ch >= 0xDC00 && ch < 0xE000)
            {
                length++;  return true;
            }
        }
        if(ch < 0x80)length++;
        else if(ch < 0x800)length += 2;
        else
        {
            if(ch >= 0xD800 && ch < 0xDC00)next = true;
            length += 3;
        }
        return true;
    }

    size_t end()
    {
        return length;
    }
};


template<typename B> struct Utf16to8Filler
{
    B &buf;
    uchar next;

    Utf16to8Filler(B &ref) : buf(ref), next(0)
    {
    }

    bool operator () (const wchar_t *ptr, size_t len)
    {
        for(; len > 0; len--)process(*ptr++);  return true;
    }

    bool operator () (wchar_t ch)
    {
        return process(ch);
    }

    bool process(uchar ch)
    {
        if(next)
        {
            if(ch >= 0xDC00 && ch < 0xE000)
            {
                ch = next | ch & 0x3FF;  next = 0;
                return buf(char(0xF0 | ch >> 18)) && buf(char(0x80 | ch >> 12 & 0x3F)) &&
                    buf(char(0x80 | ch >> 6 & 0x3F)) && buf(char(0x80 | ch & 0x3F));
            }
            else if(!buf("\xEF\xBF\xBD", 3))return false;  next = 0;
        }
        if(ch < 0x80)return buf(char(ch));
        if(ch < 0x800)return buf(char(0xC0 | ch >> 6)) && buf(char(0x80 | ch & 0x3F));
        if(ch >= 0xD800 && ch < 0xE000)
        {
            if(ch >= 0xDC00)return buf("\xEF\xBF\xBD", 3);
            next = ((ch & 0x3FF) + 0x40) << 10;  return true;
        }
        return buf(char(0xE0 | ch >> 12)) && buf(char(0x80 | ch >> 6 & 0x3F)) && buf(char(0x80 | ch & 0x3F));
    }

    bool end()
    {
        return !next || buf("\xEF\xBF\xBD", 3);
    }
};



template<typename S> class Utf8to16 : public StringLike<Utf8to16<S>, wchar_t>
{
    S src_;

public:
    template<typename T> Utf8to16(const T &str) : src_(str)
    {
    }

    Utf8to16(const char *str, size_t n) : src_(str, n)
    {
    }

    bool valid() const
    {
        return src_.valid();
    }

    size_t length() const
    {
        Utf8to16Counter cnt;  src_.fill(cnt);  return cnt.end();
    }

    template<typename B> bool fill(B &buf) const
    {
        Utf8to16Filler<B> flr(buf);  return src_.fill(flr) && flr.end();
    }
};

template<typename S> Utf8to16<const StringLike<S, char> &> utf8to16(const StringLike<S, char> &str)
{
    return Utf8to16<const StringLike<S, char> &>(str);
}

inline Utf8to16<Literal> utf8to16(const char *str, size_t n)
{
    return Utf8to16<Literal>(str, n);
}

Utf8to16<Literal> utf8to16(const char *str)
{
    return Utf8to16<Literal>(str);
}


template<typename S> class Utf16to8 : public StringLike<Utf16to8<S>, char>
{
    S src_;

public:
    template<typename T> Utf16to8(const T &str) : src_(str)
    {
    }

    Utf16to8(const wchar_t *str, size_t n) : src_(str, n)
    {
    }

    bool valid() const
    {
        return src_.valid();
    }

    size_t length() const
    {
        Utf16to8Counter cnt;  src_.fill(cnt);  return cnt.end();
    }

    template<typename B> bool fill(B &buf) const
    {
        Utf16to8Filler<B> flr(buf);  return src_.fill(flr) && flr.end();
    }
};

template<typename S> Utf16to8<const StringLike<S, wchar_t> &> utf16to8(const StringLike<S, wchar_t> &str)
{
    return Utf16to8<const StringLike<S, wchar_t> &>(str);
}

inline Utf16to8<WLiteral> utf16to8(const wchar_t *str, size_t n)
{
    return Utf16to8<WLiteral>(str, n);
}

Utf16to8<WLiteral> utf16to8(const wchar_t *str)
{
    return Utf16to8<WLiteral>(str);
}



class WString : public StringBase<wchar_t>
{
public:
    WString()
    {
    }

    WString(const WString &str) : StringBase<wchar_t>(str)
    {
    }

    template<class S> WString(const StringLike<S, wchar_t> &str) : StringBase<wchar_t>(str)
    {
    }

    WString(const wchar_t *str, size_t n) : StringBase<wchar_t>(str, n)
    {
    }

    WString(const wchar_t *str) : StringBase<wchar_t>(str)
    {
    }

    template<class S> WString(const StringLike<S, char> &str) : StringBase<wchar_t>(utf8to16(str))
    {
    }

    WString(const char *str, size_t n) : StringBase<wchar_t>(utf8to16(str, n))
    {
    }

    WString(const char *str) : StringBase<wchar_t>(utf8to16(str))
    {
    }


    friend void swap(WString &str1, WString &str2)
    {
        swap(static_cast<StringBase<wchar_t> &>(str1), static_cast<StringBase<wchar_t> &>(str2));
    }

    WString &operator = (const WString &str)
    {
        StringBase<wchar_t>::operator = (str);  return *this;
    }

    template<class S> WString &operator = (const StringLike<S, wchar_t> &str)
    {
        StringBase<wchar_t>::operator = (str);  return *this;
    }

    WString &operator = (const wchar_t *str)
    {
        return *this = LiteralBase<wchar_t>(str);
    }

    template<class S> WString &operator += (const StringLike<S, wchar_t> &str)
    {
        return *this = *this + str;
    }

    WString &operator += (const wchar_t *str)
    {
        return *this = *this + str;
    }

    template<class S> WString &operator = (const StringLike<S, char> &str)
    {
        return *this = utf8to16(str);  return *this;
    }

    WString &operator = (const char *str)
    {
        return *this = utf8to16(str);  return *this;
    }

    template<class S> WString &operator += (const StringLike<S, char> &str)
    {
        return *this = *this + utf8to16(str);
    }

    WString &operator += (const char *str)
    {
        return *this = *this + utf8to16(str);
    }


    const Utf16to8<WLiteral> utf8() const
    {
        return Utf16to8<WLiteral>(*this);
    }
};



}  // end namespace NTL_



namespace NTL
{
    using NTL_::WLiteral;
    using NTL_::utf8to16;
    using NTL_::utf16to8;
    using NTL_::WString;
}
