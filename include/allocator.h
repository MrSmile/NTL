/***************************************************************************************************
* Nothrow Template Library : allocator.h : allocator classes
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

#include "base.h"



namespace NTL_Internal_ {



template<typename A, typename T> struct Allocator
{
    // must be defined:
    // T *A::create(...);
    // void A::remove(T *);

    T *create()
    {
        return static_cast<A *>(this)->create();
    }

    template<typename T1> T *create(const T1 &arg1)
    {
        return static_cast<A *>(this)->create(arg1);
    }

    template<typename T1, typename T2> T *create(const T1 &arg1, const T2 &arg2)
    {
        return static_cast<A *>(this)->create(arg1, arg2);
    }

    template<typename T1, typename T2, typename T3> T *create(const T1 &arg1, const T2 &arg2, const T3 &arg3)
    {
        return static_cast<A *>(this)->create(arg1, arg2, arg3);
    }

    void remove(T *ptr)
    {
        static_cast<A *>(this)->remove(ptr);
    }

    class Creator
    {
        Allocator<A, T> &alloc_;

        Creator(Allocator<A, T> &alloc) : alloc_(alloc)
        {
        }

    public:
        T *operator () ()
        {
            return alloc_->create();
        }

        template<typename T1> T *operator () (const T1 &arg1)
        {
            return alloc_->create(arg1);
        }

        template<typename T1, typename T2> T *operator () (const T1 &arg1, const T2 &arg2)
        {
            return alloc_->create(arg1, arg2);
        }

        template<typename T1, typename T2, typename T3> T *operator () (const T1 &arg1, const T2 &arg2, const T3 &arg3)
        {
            return alloc_->create(arg1, arg2, arg3);
        }

    };

    class Remover
    {
        Allocator<A, T> &alloc_;

        Remover(Allocator<A, T> &alloc) : alloc_(alloc)
        {
        }

    public:
        void operator () ()
        {
            alloc_->remove();
        }
    };

    Creator creator()
    {
        return Creator(*this);
    }

    Remover remover()
    {
        return Remover(*this);
    }
};


template<typename T> struct EmptyAllocator : public Allocator<EmptyAllocator<T>, T>
{
    T *create() const
    {
        return 0;
    }

    template<typename T1> T *create(const T1 &arg1) const
    {
        return 0;
    }

    template<typename T1, typename T2> T *create(const T1 &arg1, const T2 &arg2) const
    {
        return 0;
    }

    template<typename T1, typename T2, typename T3> T *create(const T1 &arg1, const T2 &arg2, const T3 &arg3) const
    {
        return 0;
    }

    void remove(T *ptr) const
    {
    }
};


template<typename T> struct DefaultAllocator : public Allocator<DefaultAllocator<T>, T>
{
    T *create() const
    {
        return new_nt T();
    }

    template<typename T1> T *create(const T1 &arg1) const
    {
        return new_nt T(arg1);
    }

    template<typename T1, typename T2> T *create(const T1 &arg1, const T2 &arg2) const
    {
        return new_nt T(arg1, arg2);
    }

    template<typename T1, typename T2, typename T3> T *create(const T1 &arg1, const T2 &arg2, const T3 &arg3) const
    {
        return new_nt T(arg1, arg2, arg3);
    }

    void remove(T *ptr) const
    {
        delete ptr;
    }
};



}  // end namespace NTL_Internal_



namespace NTL
{
    using NTL_Internal_::Allocator;
    using NTL_Internal_::EmptyAllocator;
    using NTL_Internal_::DefaultAllocator;
}
