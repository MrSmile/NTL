/***************************************************************************************************
* Nothrow Template Library : alloc.h : custom allocator for test suite
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

#include "base.h"
#include <cstdlib>



struct MemoryHandler
{
    int n_new, n_del;
    int cur, fail;

    MemoryHandler() : n_new(0), n_del(0), cur(0), fail(0)
    {
    }

    void reset()
    {
        n_new = n_del = 0;
    }

    void check(int tg_new, int tg_del)
    {
        assert(n_new == tg_new && n_del == tg_del);
        n_new = n_del = 0;
    }


    void make_fail(int n)
    {
        cur = 0;  fail = n;
    }

    void make_reliable()
    {
        cur = fail = 0;
    }

    void *alloc_throw(size_t size)
    {
        assert(false);  void *res;
        if((res = std::malloc(size)))n_new++;
        return res;
    }

    void *alloc(size_t size)
    {
        if(fail && ++cur >= fail)
        {
            cur = 0;  return 0;
        }
        void *res = std::malloc(size);
        if(res)n_new++;  else assert(false);
        return res;
    }

    void free(void *ptr)
    {
        if(!ptr)return;  n_del++;  std::free(ptr);
    }
};

extern MemoryHandler mem_handler;


inline void *operator new (size_t size) throw()
{
    return mem_handler.alloc_throw(size);
}

inline void *operator new[] (size_t size) throw()
{
    return mem_handler.alloc_throw(size);
}

inline void *operator new (size_t size, const std::nothrow_t &) throw()
{
    return mem_handler.alloc(size);
}

inline void *operator new[] (size_t size, const std::nothrow_t &) throw()
{
    return mem_handler.alloc(size);
}

inline void operator delete (void *ptr)
{
    mem_handler.free(ptr);
}

inline void operator delete[] (void *ptr)
{
    mem_handler.free(ptr);
}
