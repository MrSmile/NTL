/***************************************************************************************************
* Nothrow Template Library : base.h : common include file
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


#include <new>
#include <algorithm>
#include <cstring>
#include <cstddef>



#if defined _DEBUG || defined DEBUG
#define NTL_DEBUG
#else 
#undef NTL_DEBUG
#endif



#ifdef _MSC_VER

#define NTL_UNUSED

namespace NTL_
{
    inline void debug_break()
    {
        __asm int 3;
    }
}

#elif defined __GNUC__

#define NTL_UNUSED  __attribute__((unused))

namespace NTL_
{
    inline void debug_break()
    {
        asm("int $3");
    }
}

#else

#define NTL_UNUSED

#include <cstdlib>

namespace NTL_
{
    inline void debug_break()
    {
        std::abort();
    }
}

#endif


#undef assert

#ifdef NTL_DEBUG

#include <iostream>

namespace NTL_
{
    inline void ntl_assert(bool test, const char *msg)
    {
        if(test)return;  std::cout << msg;  debug_break();
    }
}

#define NTL_STRING(str) #str
#define NTL_ASSERT(expr, file, line) \
    NTL_::ntl_assert(expr, "Assert failed: " #expr "; file " file "; line " NTL_STRING(line) ".\n")
#define assert(expr) NTL_ASSERT(expr, __FILE__, __LINE__)

#else

namespace NTL_
{
    inline void ntl_assert()
    {
    }
}

#define assert(expr) NTL_::ntl_assert()

#endif



#if defined _MSC_VER

#include <intrin.h>
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma warning(disable: 4800)

namespace NTL_
{
    inline size_t sync_inc(volatile size_t &n)
    {
        return size_t(_InterlockedExchangeAdd(reinterpret_cast<volatile long *>(&n), 1));
    }

    inline size_t sync_dec(volatile size_t &n)
    {
        return size_t(_InterlockedExchangeAdd(reinterpret_cast<volatile long *>(&n), -1));
    }
}

#elif defined __GNUC__

namespace NTL_
{
    inline size_t sync_inc(volatile size_t &n)
    {
        return __sync_fetch_and_add(&n, size_t(1));
    }

    inline size_t sync_dec(volatile size_t &n)
    {
        return __sync_fetch_and_sub(&n, size_t(1));
    }
}

#else  // Not thread safe!!!

namespace NTL_
{
    inline size_t sync_inc(volatile size_t &n)
    {
        n++;
    }

    inline size_t sync_dec(volatile size_t &n)
    {
        n--;
    }
}

#endif



namespace NTL_ {



#define new_nt new(std::nothrow)

using std::swap;



#ifdef NTL_DEBUG

struct invalid_ptr_t
{
    template<typename T> operator T * () const
    {
        return reinterpret_cast<T *>(0x12345678);
    }
};

static invalid_ptr_t invalid_ptr NTL_UNUSED;

#endif



class Invalid  // Non instantable
{
    Invalid();
};


class Heavy  // Non copyable
{
    Heavy(const Heavy &);
    Heavy &operator = (const Heavy &);

public:
    Heavy()
    {
    }
};



template<bool F, typename T> struct EnableIf
{
    typedef T type;
};

template<typename T> struct EnableIf<false, T>
{
};

template<typename S, typename D> struct IsConvertable
{
    typedef char true_type[1];
    typedef char false_type[2];

    static true_type &check(D);
    static false_type &check(...);
    static S src;

    enum
    {
        value = (sizeof(check(src)) == sizeof(true_type))
    };
};


template<typename T> struct Comparable;

template<typename T> struct IsComparable
{
    enum
    {
        value = IsConvertable<const T *, const Comparable<T> *>::value
    };
};

template<typename T> struct Comparable
{
    // must be defined:
    // int T::cmp(const T1 &obj) const;

    template<typename T1> int cmp(const T1 &obj) const
    {
        return static_cast<const T *>(this)->cmp(obj);
    }

    template<typename T1> friend bool operator == (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) == 0;
    }

    template<typename T1> friend bool operator != (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) != 0;
    }

    template<typename T1> friend bool operator >= (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) >= 0;
    }

    template<typename T1> friend bool operator <= (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) <= 0;
    }

    template<typename T1> friend bool operator > (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) > 0;
    }

    template<typename T1> friend bool operator < (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) < 0;
    }

    template<typename T1> friend typename EnableIf<!IsComparable<T1>::value, bool>::type operator == (const T1 &obj1, const T &obj2)
    {
        return 0 == obj2.cmp(obj1);
    }

    template<typename T1> friend typename EnableIf<!IsComparable<T1>::value, bool>::type operator != (const T1 &obj1, const T &obj2)
    {
        return 0 != obj2.cmp(obj1);
    }

    template<typename T1> friend typename EnableIf<!IsComparable<T1>::value, bool>::type operator >= (const T1 &obj1, const T &obj2)
    {
        return 0 >= obj2.cmp(obj1);
    }

    template<typename T1> friend typename EnableIf<!IsComparable<T1>::value, bool>::type operator <= (const T1 &obj1, const T &obj2)
    {
        return 0 <= obj2.cmp(obj1);
    }

    template<typename T1> friend typename EnableIf<!IsComparable<T1>::value, bool>::type operator > (const T1 &obj1, const T &obj2)
    {
        return 0 > obj2.cmp(obj1);
    }

    template<typename T1> friend typename EnableIf<!IsComparable<T1>::value, bool>::type operator < (const T1 &obj1, const T &obj2)
    {
        return 0 < obj2.cmp(obj1);
    }
};



}  // end namespace NTL_



namespace NTL
{
    using std::swap;

#ifdef NTL_DEBUG
    using NTL_::invalid_ptr;
#endif

    using NTL_::sync_inc;
    using NTL_::sync_dec;

    using NTL_::Invalid;
    using NTL_::Heavy;

    using NTL_::EnableIf;
    using NTL_::IsConvertable;
    using NTL_::Comparable;

    using NTL_::debug_break;
}
