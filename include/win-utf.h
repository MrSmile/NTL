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



namespace NTL_Internal_ {



template<> inline LiteralBase<wchar_t>::LiteralBase(const wchar_t *str) :
    ptr_(str), len_(str ? std::wcslen(str) : 0)
{
}

template<> inline int LiteralBase<wchar_t>::cmp(const LiteralBase &str) const
{
    if(!ptr_)return str.ptr_ ? -1 : 0;  if(!str.ptr_)return 1;
    if(len_ < str.len_)return std::wmemcmp(ptr_, str.ptr_, len_) > 0 ? 1 : -1;
    if(len_ > str.len_)return std::wmemcmp(ptr_, str.ptr_, str.len_) < 0 ? -1 : 1;
    return std::wmemcmp(ptr_, str.ptr_, len_);
}



class WStringConv : public StringLike<WStringConv, char>, public StringBase<wchar_t>
{
    typedef unsigned long uchar_;


    struct Counter_
    {
        size_t length;
        uchar_ next, lim;
        unsigned count;

        Counter_() : length(0), count(0)
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


    struct Buffer_
    {
        wchar_t *buf;
        uchar_ next, lim;
        unsigned count;

        Buffer_(wchar_t *ptr) : buf(ptr), count(0)
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
                if(count)*buf++ = 0xFFFD;  count = 0;  *buf++ = ch;
            }
            else if(!(ch & 0x40))
            {
                if(count)
                {
                    next = next << 6 | ch & 0x3F;
                    if(!--count)
                    {
                        if(next < lim)*buf++ = 0xFFFD;
                        else if(next < 0xE000)
                            if(next < 0xD800)*buf++ = next;
                            else *buf++ = 0xFFFD;
                        else if(next < 0x10000)*buf++ = next;
                        else if(next < 0x11000)
                        {
                            next -= 0x10000;
                            *buf++ = 0xD800 | next >> 10;
                            *buf++ = 0xDC00 | next & 0x3FF;
                        }
                        else *buf++ = 0xFFFD;
                    }
                }
                else *buf++ = 0xFFFD;
            }
            else
            {
                if(count)*buf++ = 0xFFFD;
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
                    *buf++ = 0xFFFD;  count = 0;
                }
            }
            return true;
        }

        void end()
        {
            if(count)*buf++ = 0xFFFD;  *buf = 0;
        }
    };


    template<class S> static size_t count_(const StringLike<S, char> &str)
    {
        if(!str.valid())return 0;  Counter_ buf;  str.fill(buf);  return buf.end() + 1;
    }

    template<class S> void copy_(const StringLike<S, char> &str, size_t offs = 0)
    {
        if(!valid())return;  Buffer_ buf(buffer() + offs);  str.fill(buf);  buf.end();
    }


public:
    WStringConv()
    {
    }

    WStringConv(const WStringConv &str) : StringBase<wchar_t>(str)
    {
    }

    template<class S> WStringConv(const StringLike<S, wchar_t> &str) : StringBase<wchar_t>(str)
    {
    }

    template<class S> WStringConv(const StringLike<S, char> &str) : StringBase<wchar_t>(count_(str))
    {
        copy_(str);
    }

    template<class S> StringBase<wchar_t> &operator = (const StringLike<S, char> &str)
    {
        if(resize(count_(str)))copy_(str);  return *this;
    }

    template<class S> StringBase<wchar_t> &operator += (const StringLike<S, char> &str)
    {
        size_t offs = StringBase<wchar_t>::length(), len = count_(str);
        StringBase<wchar_t> tmp;  swap(tmp, *static_cast<StringBase<wchar_t> *>(this));
        if(len && tmp.valid() && resize(offs + len))
        {
            std::memcpy(buffer(), tmp.data(), offs * sizeof(wchar_t));  copy_(str, offs);
        }
        return *this;
    }


    bool valid() const
    {
        return StringBase<wchar_t>::valid();
    }

    size_t length() const
    {
        if(!valid())return 0;  const wchar_t *ptr = buffer();  size_t count = 0;
        for(size_t len = StringBase<wchar_t>::length(); len > 0; len--)
        {
            uchar_ ch = *ptr++;
            if(ch < 0x80)count++;
            else if(ch < 0x800)count += 2;
            else if(ch < 0xD800 || ch >= 0xE000)count += 3;
            else if(ch < 0xDC00 && uchar_(*ptr) >= 0xDC00 && uchar_(*ptr) < 0xE000)
            {
                count += 4;  ptr++;  len--;
            }
            else count += 3;
        }
        return count;
    }

    template<typename B> bool fill(B &buf) const
    {
        if(!valid())return true;  const wchar_t *ptr = buffer();
        for(size_t len = StringBase<wchar_t>::length(); len > 0; len--)
        {
            uchar_ ch = *ptr++;
            if(ch >= 0x80)
            {
                if(ch >= 0x800)
                {
                    if(ch >= 0xD800 && ch < 0xE000)
                    {
                        uchar_ ch1 = *ptr;
                        if(ch >= 0xDC00 || ch1 < 0xDC00 || ch1 >= 0xE000)
                        {
                            if(!buf("\xEF\xBF\xBD", 3))return false;  continue;
                        }
                        else
                        {
                            ptr++;  len--;
                            ch = ((ch & 0x3FF) + 0x40) << 10 | ch1 & 0x3FF;
                            if(!buf(char(0xF0 | ch >> 18)))return false;
                            if(!buf(char(0x80 | ch >> 12 & 0x3F)))return false;;
                        }
                    }
                    else if(!buf(char(0xE0 | ch >> 12)))return false;
                    if(!buf(char(0x80 | ch >> 6 & 0x3F)))return false;
                }
                else if(!buf(char(0xC0 | ch >> 6)))return false;
                if(!buf(char(0x80 | ch & 0x3F)))return false;
            }
            else if(!buf(char(ch)))return false;
        }
        return true;
    }
};



class WString : public WStringConv
{
public:
    WString()
    {
    }

    WString(const WString &str) : WStringConv(str)
    {
    }

    template<class S> WString(const StringLike<S, wchar_t> &str) : WStringConv(str)
    {
    }

    WString(const wchar_t *str, size_t n) : WStringConv(LiteralBase<wchar_t>(str, n))
    {
    }

    WString(const wchar_t *str) : WStringConv(LiteralBase<wchar_t>(str))
    {
    }

    template<class S> WString(const StringLike<S, char> &str) : WStringConv(str)
    {
    }

    WString(const char *str, size_t n) : WStringConv(LiteralBase<char>(str, n))
    {
    }

    WString(const char *str) : WStringConv(LiteralBase<char>(str))
    {
    }


    WString &operator = (const WString &str)
    {
        WStringConv::operator = (str);  return *this;
    }

    template<class S> WString &operator = (const StringLike<S, wchar_t> &str)
    {
        WStringConv::operator = (str);  return *this;
    }

    WString &operator = (const wchar_t *str)
    {
        return *this = LiteralBase<wchar_t>(str);
    }

    template<class S> WString &operator = (const StringLike<S, char> &str)
    {
        WStringConv::operator = (str);  return *this;
    }

    WString &operator = (const char *str)
    {
        return *this = Literal(str);
    }

    template<class S> WString &operator += (const StringLike<S, char> &str)
    {
        WStringConv::operator += (str);  return *this;
    }

    WString &operator += (const char *str)
    {
        return *this += Literal(str);
    }


    size_t length() const
    {
        return StringBase<wchar_t>::length();
    }

    template<typename B> bool fill(B &buf) const
    {
        return StringBase<wchar_t>::fill(buf);
    }
};



}  // end namespace NTL_Internal_



namespace NTL
{
    using NTL_Internal_::WString;
}
