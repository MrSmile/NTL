/***************************************************************************************************
* Nothrow Template Library : fmt-int.h : integer to string conversions
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

#include "format.h"
#include <climits>



namespace NTL_ {



#ifdef ULLONG_MAX
typedef unsigned long long topint;
#else
typedef unsigned long topint;
#endif


template<topint N> struct IntDecOrder
{
    enum { value = IntDecOrder<N / 10>::value + 1 };
};

template<> struct IntDecOrder<0>
{
    enum { value = 0 };
};


template<topint N1, topint N2> struct Max
{
    enum { value = N1 > N2 ? N1 : N2 };
};


template<typename Tt, typename Tf, bool flag> struct Choose
{
    typedef Tt type;
};

template<typename Tt, typename Tf> struct Choose<Tt, Tf, false>
{
    typedef Tf type;
};



template<typename T> struct IntTraits
{
};

struct SignedCharTraits
{
    typedef char is_sint;
};

struct UnsignedCharTraits
{
    typedef char is_uint;
};

template<> struct IntTraits<char> : public Choose<SignedCharTraits, UnsignedCharTraits, CHAR_MIN>::type
{
    typedef unsigned char utype;
    enum { dec_order = IntDecOrder<Max<CHAR_MAX, utype(-utype(CHAR_MIN))>::value>::value };
};

template<> struct IntTraits<signed char>
{
    typedef signed char is_sint;
    typedef unsigned char utype;
    enum { dec_order = IntDecOrder<Max<SCHAR_MAX, utype(-utype(SCHAR_MIN))>::value>::value };
};

template<> struct IntTraits<unsigned char>
{
    typedef unsigned char is_uint;
    typedef unsigned char utype;
    enum { dec_order = IntDecOrder<UCHAR_MAX>::value };
};

template<> struct IntTraits<short>
{
    typedef short is_sint;
    typedef unsigned short utype;
    enum { dec_order = IntDecOrder<Max<SHRT_MAX, utype(-utype(SHRT_MIN))>::value>::value };
};

template<> struct IntTraits<unsigned short>
{
    typedef unsigned short is_uint;
    typedef unsigned short utype;
    enum { dec_order = IntDecOrder<USHRT_MAX>::value };
};

template<> struct IntTraits<int>
{
    typedef int is_sint;
    typedef unsigned utype;
    enum { dec_order = IntDecOrder<Max<INT_MAX, utype(-utype(INT_MIN))>::value>::value };
};

template<> struct IntTraits<unsigned>
{
    typedef unsigned is_uint;
    typedef unsigned utype;
    enum { dec_order = IntDecOrder<UINT_MAX>::value };
};

template<> struct IntTraits<long>
{
    typedef long is_sint;
    typedef unsigned long utype;
    enum { dec_order = IntDecOrder<Max<LONG_MAX, utype(-utype(LONG_MIN))>::value>::value };
};

template<> struct IntTraits<unsigned long>
{
    typedef unsigned long is_uint;
    typedef unsigned long utype;
    enum { dec_order = IntDecOrder<ULONG_MAX>::value };
};

#ifdef ULLONG_MAX

template<> struct IntTraits<long long>
{
    typedef long long is_sint;
    typedef unsigned long long utype;
    enum { dec_order = IntDecOrder<Max<LLONG_MAX, utype(-utype(LLONG_MIN))>::value>::value };
};

template<> struct IntTraits<unsigned long long>
{
    typedef unsigned long long is_uint;
    typedef unsigned long long utype;
    enum { dec_order = IntDecOrder<ULLONG_MAX>::value };
};

#endif


template<typename T> class UIntDec : public StringLike<UIntDec<T>, char>
{
    size_t pos;
    char data[IntTraits<T>::dec_order];

public:
    UIntDec(typename IntTraits<T>::utype num) : pos(IntTraits<T>::dec_order)
    {
        do
        {
            data[--pos] = '0' + num % 10;  num /= 10;
        }
        while(num);
    }

    bool valid() const
    {
        return true;
    }

    size_t length() const
    {
        return IntTraits<T>::dec_order - pos;
    }

    template<typename B> bool fill(B &buf) const
    {
        return buf(data + pos, IntTraits<T>::dec_order - pos);
    }
};


template<typename T> typename IntTraits<T>::utype iabs(T num)
{
    return num < 0 ? (typename IntTraits<T>::utype)-(typename IntTraits<T>::utype)num : num;
}

template<typename T> class SIntDec : public StringLike<SIntDec<T>, char>
{
    Literal sign;
    UIntDec<T> base;

public:
    SIntDec(T num, const Literal minus = Literal("-", 1), const Literal plus = Literal("", 0)) :
        sign(num < 0 ? minus : plus), base(iabs(num))
    {
    }

    bool valid() const
    {
        return sign.valid() && base.valid();
    }

    size_t length() const
    {
        return sign.length() + base.length();
    }

    template<typename B> bool fill(B &buf) const
    {
        return sign.fill(buf) && base.fill(buf);
    }
};



template<char ch, typename T> Argument<UIntDec<typename IntTraits<T>::is_uint>, char, ch> dec(T num)
{
    return Argument<UIntDec<T>, char, ch>(num);
}

template<typename T> Argument<UIntDec<typename IntTraits<T>::is_uint>, char, '\0'> dec(T num)
{
    return Argument<UIntDec<T>, char, '\0'>(num);
}

template<char ch, typename T> Argument<SIntDec<typename IntTraits<T>::is_sint>, char, ch>
    dec(T num, const Literal minus = Literal("-", 1), const Literal plus = Literal("", 0))
{
    return Argument<SIntDec<T>, char, ch>(num, minus, plus);
}

template<typename T> Argument<SIntDec<typename IntTraits<T>::is_sint>, char, '\0'>
    dec(T num, const Literal minus = Literal("-", 1), const Literal plus = Literal("", 0))
{
    return Argument<SIntDec<T>, char, '\0'>(num, minus, plus);
}



}  // end namespace NTL_



namespace NTL
{
    using NTL_::dec;
}
