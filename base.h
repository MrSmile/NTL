// base.h -- common include file of the No Throw Library
//

#pragma once


#include <new>
#include <algorithm>
#include <cstring>
#include <cstddef>



#if defined _DEBUG || defined DEBUG

#define NTL_DEBUG
#undef NDEBUG

struct invalid_ptr_t
{
    template<typename T> operator T * () const
    {
        return reinterpret_cast<T *>(0x12345678);
    }
};

static invalid_ptr_t invalid_ptr;

#else

#define NDEBUG

#endif


#define new_nt new(std::nothrow)


using std::swap;
using std::memcpy;
using std::memset;


#ifdef NTL_DEBUG

#include <iostream>

#if defined _MSC_VER

inline void ntl_assert_(bool test, const char *msg)
{
    if(test)return;  std::cout << msg;  __asm int 3;
}

#elif defined __GNUC__

inline void ntl_assert_(bool test, const char *msg)
{
    if(test)return;  std::cout << msg;  asm("int $3");
}

#else

#include <cstdlib>

inline void ntl_assert_(bool test, const char *msg)
{
    if(test)return;  std::cout << msg;  abort();
}

#endif

#define NTL_STRING(str) #str
#define NTL_ASSERT(expr, file, line) ntl_assert_(expr, "Assert failed: " #expr "; file " file "; line " NTL_STRING(line) ".\n")
#define assert(expr) NTL_ASSERT(expr, __FILE__, __LINE__)

#else

inline void ntl_assert_()
{
}

#define assert(expr) ntl_assert_()

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
