// pointer.h -- smart pointers
//

#pragma once

#include "base.h"



template<typename T> class Pointer : public Heavy
{
    T *target_;


public:
    Pointer(T *ptr = 0) : target_(ptr)
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
        ::swap(ptr1.target_, ptr2.target_);
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
    Array(T *ptr = 0) : target_(ptr)
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
        ::swap(ptr1.target_, ptr2.target_);
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


