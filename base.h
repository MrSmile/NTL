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



#ifdef _MSC_VER

namespace NTL_Internal_
{
    inline void debug_break()
    {
        __asm int 3;
    }
}

#elif defined __GNUC__

namespace NTL_Internal_
{
    inline void debug_break()
    {
        asm("int $3");
    }
}

#else

#include <cstdlib>

namespace NTL_Internal_
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

namespace NTL_Internal_
{
    inline void ntl_assert(bool test, const char *msg)
    {
        if(test)return;  std::cout << msg;  debug_break();
    }
}

#define NTL_STRING(str) #str
#define NTL_ASSERT(expr, file, line) \
    NTL_Internal_::ntl_assert(expr, "Assert failed: " #expr "; file " file "; line " NTL_STRING(line) ".\n")
#define assert(expr) NTL_ASSERT(expr, __FILE__, __LINE__)

#else

namespace NTL_Internal_
{
    inline void ntl_assert()
    {
    }
}

#define assert(expr) NTL_Internal_::ntl_assert()

#endif



#if defined _MSC_VER

#include <intrin.h>
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma warning(disable: 4800)

namespace NTL_Internal_
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

namespace NTL_Internal_
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

namespace NTL_Internal_
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



namespace NTL_Internal_ {



#define new_nt new(std::nothrow)

using std::swap;



#if defined _DEBUG || defined DEBUG

#define NTL_DEBUG

struct invalid_ptr_t
{
    template<typename T> operator T * () const
    {
        return reinterpret_cast<T *>(0x12345678);
    }
};

static invalid_ptr_t invalid_ptr;

#endif



class Invalid  // Non instantable
{
    Invalid();
};


class Heavy  // Non copyable
{
    Heavy(Heavy &);
    Heavy &operator = (Heavy &);

public:
    Heavy()
    {
    }
};



template<typename T, typename T1 = T> struct Comparable
{
    // must be defined:
    // int T::cmp(const T1 &obj) const;

    int cmp(const T1 &obj) const
    {
        return static_cast<const T *>(this)->cmp(obj);
    }

    friend bool operator == (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) == 0;
    }

    friend bool operator != (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) != 0;
    }

    friend bool operator >= (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) >= 0;
    }

    friend bool operator <= (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) <= 0;
    }

    friend bool operator > (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) > 0;
    }

    friend bool operator < (const T &obj1, const T1 &obj2)
    {
        return obj1.cmp(obj2) < 0;
    }

    friend bool operator == (const T1 &obj1, const T &obj2)
    {
        return 0 == obj2.cmp(obj1);
    }

    friend bool operator != (const T1 &obj1, const T &obj2)
    {
        return 0 != obj2.cmp(obj1);
    }

    friend bool operator >= (const T1 &obj1, const T &obj2)
    {
        return 0 >= obj2.cmp(obj1);
    }

    friend bool operator <= (const T1 &obj1, const T &obj2)
    {
        return 0 <= obj2.cmp(obj1);
    }

    friend bool operator > (const T1 &obj1, const T &obj2)
    {
        return 0 > obj2.cmp(obj1);
    }

    friend bool operator < (const T1 &obj1, const T &obj2)
    {
        return 0 < obj2.cmp(obj1);
    }
};

template<typename T> struct Comparable<T, T>
{
    int cmp(const T &obj) const
    {
        return static_cast<const T *>(this)->cmp(obj);
    }

    friend bool operator == (const T &obj1, const T &obj2)
    {
        return obj1.cmp(obj2) == 0;
    }

    friend bool operator != (const T &obj1, const T &obj2)
    {
        return obj1.cmp(obj2) != 0;
    }

    friend bool operator > (const T &obj1, const T &obj2)
    {
        return obj1.cmp(obj2) > 0;
    }

    friend bool operator < (const T &obj1, const T &obj2)
    {
        return obj1.cmp(obj2) < 0;
    }

    friend bool operator >= (const T &obj1, const T &obj2)
    {
        return obj1.cmp(obj2) >= 0;
    }

    friend bool operator <= (const T &obj1, const T &obj2)
    {
        return obj1.cmp(obj2) <= 0;
    }
};



}  // end namespace NTL_Internal_



namespace NTL
{
    using std::swap;

#ifdef NTL_DEBUG
    using NTL_Internal_::invalid_ptr;
#endif

    using NTL_Internal_::sync_inc;
    using NTL_Internal_::sync_dec;

    using NTL_Internal_::Invalid;
    using NTL_Internal_::Heavy;

    using NTL_Internal_::Comparable;

    using NTL_Internal_::debug_break;
}
