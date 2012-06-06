/***************************************************************************************************
* Nothrow Template Library : pointer.h : exclusive smart pointers
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



namespace NTL_ {



template<typename T> class Pointer : public Heavy
{
    T *target_;


public:
    Pointer() : target_(0)
    {
    }

    explicit Pointer(T *ptr) : target_(ptr)
    {
    }

    void clear()
    {
        delete target_;  target_ = 0;
    }

    ~Pointer()
    {
        delete target_;
    }


    T *attach(T *ptr)
    {
        assert(target_ == 0);  target_ = ptr;  return ptr;
    }

    T *set(T *ptr)
    {
        if(target_ == ptr)return ptr;  delete target_;  target_ = ptr;  return ptr;
    }

    Pointer<T> &operator = (T *ptr)
    {
        set(ptr);  return *this;
    }

    T *detach()
    {
        T *ptr = target_;  target_ = 0;  return ptr;
    }

    friend void swap(Pointer<T> &ptr1, Pointer<T> &ptr2)
    {
        swap(ptr1.target_, ptr2.target_);
    }


    const T *ptr() const
    {
        return target_;
    }

    T *ptr()
    {
        return target_;
    }

    operator T *()
    {
        return target_;
    }

    T *operator -> ()
    {
        return target_;
    }

    T &operator * ()
    {
        return *target_;
    }
};


template<typename T> class Array : public Heavy
{
    T *target_;


public:
    Array() : target_(0)
    {
    }

    explicit Array(T *ptr) : target_(ptr)
    {
    }

    explicit Array(size_t n) : target_(n ? new_nt T[n] : 0)
    {
    }

    bool alloc(size_t n)
    {
        assert(target_ == 0);  return !n || (target_ = new_nt T[n]);
    }

    void clear()
    {
        delete [] target_;  target_ = 0;
    }

    ~Array()
    {
        delete [] target_;
    }


    T *attach(T *ptr)
    {
        assert(target_ == 0);  target_ = ptr;  return ptr;
    }

    Array<T> &operator = (T *ptr)
    {
        attach(ptr);  return *this;
    }

    T *detach()
    {
        T *ptr = target_;  target_ = 0;  return ptr;
    }

    friend void swap(Array<T> &ptr1, Array<T> &ptr2)
    {
        swap(ptr1.target_, ptr2.target_);
    }


    const T *ptr() const
    {
        return target_;
    }

    T *ptr()
    {
        return target_;
    }

    operator T *()
    {
        return target_;
    }
};



}  // end namespace NTL_



namespace NTL
{
    using NTL_::Pointer;
    using NTL_::Array;
}
