// base.h -- common include file of the No Throw Library
//

#pragma once


#include <new>
#include <algorithm>
#include <cstring>
#include <cstddef>



#define new_nt new(std::nothrow)


using std::swap;
using std::memcpy;
using std::memset;


#if !defined _DEBUG && !defined DEBUG

inline void assert(bool test)
{
}

#elif defined _MSC_VER

inline void assert(bool test)
{
    if(!test)__asm int 3;
}

#elif defined __GNUC__

inline void assert(bool test)
{
    if(!test)asm("int $3");
}

#else

#include <cassert>

inline void assert(bool test)
{
    return std::assert(test);
}

#endif



#if defined _MSC_VER

#include <intrin.h>
#pragma intrinsic(_InterlockedExchangeAdd)

inline size_t sync_inc(volatile size_t &n)
{
    return size_t(_InterlockedExchangeAdd(reinterpret_cast<volatile long *>(&n), 1));
}

inline size_t sync_dec(volatile size_t &n)
{
    return size_t(_InterlockedExchangeAdd(reinterpret_cast<volatile long *>(&n), -1));
}

#elif defined __GNUC__

inline size_t sync_inc(volatile size_t &n)
{
    return __sync_fetch_and_add(&n, size_t(1));
}

inline size_t sync_dec(volatile size_t &n)
{
    return __sync_fetch_and_sub(&n, size_t(1));
}

#else  // Not thread safe!!!

inline size_t sync_inc(volatile size_t &n)
{
    n++;
}

inline size_t sync_dec(volatile size_t &n)
{
    n--;
}

#endif



class Heavy  // Non copyable
{
    Heavy(Heavy &);
    Heavy &operator = (Heavy &);

public:
    Heavy()
    {
    }
};


template<typename T> struct Comparable
{
    // must be defined: 
    // int T::cmp(const T &obj) const;

    int cmp(const Comparable<T> &obj) const
    {
        return static_cast<const T *>(this)->cmp(static_cast<const T &>(obj));
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
