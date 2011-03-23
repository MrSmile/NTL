// literal.h -- string classes
//

#pragma once

#include "base.h"



template<typename C> class Character;
template<typename C> class StringBase;
template<typename C> class LiteralBase;
template<class S1, class S2, typename C> class Concatenation;

template<class S, typename C> struct StringLike
{
    // must be defined:
    // bool S::valid() const;
    // size_t S::length() const;
    // void S::fill(C *&buf) const;
    // void S::fill(C *&buf, size_t &max, size_t &start) const;

    bool valid() const
    {
        return static_cast<const S *>(this)->valid();
    }

    size_t length() const
    {
        return static_cast<const S *>(this)->length();
    }

    void fill(C *&buf) const
    {
        return static_cast<const S *>(this)->fill(buf);
    }

    void fill(C *&buf, size_t &max, size_t &start) const
    {
        return static_cast<const S *>(this)->fill(buf, max, start);
    }

    template<typename S1> Concatenation<const S &, const S1 &, C> operator + (const StringLike<S1, C> &str) const;
    Concatenation<const S &, LiteralBase<C>, C> operator + (const C *str) const;
    Concatenation<const S &, Character<C>, C> operator + (C ch) const;
};

template<typename S1, typename S2, typename C> class Concatenation : public StringLike<Concatenation<S1, S2, C>, C>
{
    S1 op1_;
    S2 op2_;

public:
    template<typename T1, typename T2> Concatenation(const T1 &str1, const T2 &str2) : op1_(str1), op2_(str2)
    {
    }

    bool valid() const
    {
        return op1_.valid() && op2_.valid();
    }

    size_t length() const
    {
        return op1_.length() + op2_.length();
    }

    void fill(C *&buf) const
    {
        op1_.fill(buf);  op2_.fill(buf);
    }

    void fill(C *&buf, size_t &max, size_t &start) const
    {
        op1_.fill(buf, max, start);  op2_.fill(buf, max, start);
    }
};

template<typename S, typename C> template<typename S1> inline Concatenation<const S &, const S1 &, C>
    StringLike<S, C>::operator + (const StringLike<S1, C> &str) const
{
    return Concatenation<const S &, const S1 &, C>(*static_cast<const S *>(this), static_cast<const S1 &>(str));
}

template<typename S, typename C> inline Concatenation<const S &, LiteralBase<C>, C>
    StringLike<S, C>::operator + (const C *str) const
{
    return Concatenation<const S &, LiteralBase<C>, C>(*static_cast<const S *>(this), str);
}

template<typename S, typename C> inline Concatenation<LiteralBase<C>, const S &, C>
    operator + (const C *str1, const StringLike<S, C> &str2)
{
    return Concatenation<LiteralBase<C>, const S &, C>(str1, static_cast<const S &>(str2));
}

template<typename S, typename C> inline Concatenation<const S &, Character<C>, C>
    StringLike<S, C>::operator + (C ch) const
{
    return Concatenation<const S &, Character<C>, C>(*static_cast<const S *>(this), ch);
}

template<typename S, typename C> inline Concatenation<Character<C>, const S &, C>
    operator + (C ch1, const StringLike<S, C> &str2)
{
    return Concatenation<Character<C>, const S &, C>(ch1, static_cast<const S &>(str2));
}


template<typename C> class Character : public StringLike<Character<C>, C>
{
    C ch_;


public:
    Character(const Character &ch) : ch_(ch.ch_)
    {
    }

    Character(C ch) : ch_(ch)
    {
    }


    bool valid() const
    {
        return true;
    }

    size_t length() const
    {
        return 1;
    }

    void fill(C *&buf) const
    {
        *buf++ = ch_;
    }

    void fill(C *&buf, size_t &max, size_t &start) const
    {
        if(start)
        {
            start--;  return;
        }
        if(max)
        {
            *buf = ch_;  max--;
        }
        buf++;
    }
};


template<typename C> class LiteralBase : public StringLike<LiteralBase<C>, C>, public Comparable<LiteralBase<C> >
{
    const C *ptr_;
    size_t len_;


public:
    LiteralBase() : ptr_(0), len_(0)
    {
    }

    LiteralBase(const LiteralBase &str) : ptr_(str.ptr_), len_(str.len_)
    {
    }

    LiteralBase(const C *str, size_t n) : ptr_(str), len_(n)
    {
    }

    LiteralBase(const C *str) : ptr_(str), len_(0)
    {
        if(str)while(str[len_])len_++;
    }


    C operator [] (size_t index) const
    {
        assert(index < len_);  return ptr_[index];
    }

    LiteralBase<C> substr(size_t beg, size_t n = size_t(-1)) const
    {
        if(beg > len_)beg = len_;  if(n > len_ - beg)n = len_ - beg;
        return LiteralBase<C>(ptr_ + beg, n);
    }

    size_t scan(C ch, size_t pos = 0) const
    {
        for(;; pos++)
        {
            if(pos >= len_)return size_t(-1);  if(ptr_[pos] == ch)return pos;
        }
    }

    size_t rscan(C ch, size_t pos) const
    {
        for(;; pos--)
        {
            if(pos >= len_)return size_t(-1);  if(ptr_[pos] == ch)return pos;
        }
    }

    size_t rscan(C ch) const
    {
        for(size_t pos = len_ - 1;; pos--)
        {
            if(pos >= len_)return size_t(-1);  if(ptr_[pos] == ch)return pos;
        }
    }

    int cmp(const LiteralBase &str) const
    {
        if(!ptr_)return str.ptr_ ? -1 : 0;  if(!str.ptr_)return 1;
        if(len_ < str.len_)
        {
            for(size_t i = 0; i < len_; i++)
            {
                if(ptr_[i] < str.ptr_[i])return -1;  if(ptr_[i] > str.ptr_[i])return 1;
            }
            return -1;
        }
        if(len_ > str.len_)
        {
            for(size_t i = 0; i < str.len_; i++)
            {
                if(ptr_[i] < str.ptr_[i])return -1;  if(ptr_[i] > str.ptr_[i])return 1;
            }
            return 1;
        }
        for(size_t i = 0; i < len_; i++)
        {
            if(ptr_[i] < str.ptr_[i])return -1;  if(ptr_[i] > str.ptr_[i])return 1;
        }
        return 0;
    }


    bool valid() const
    {
        return ptr_;
    }

    const C *data() const
    {
        return ptr_;
    }

    size_t length() const
    {
        return len_;
    }

    void fill(C *&buf) const
    {
        memcpy(buf, ptr_, len_ * sizeof(C));  buf += len_;
    }

    void fill(C *&buf, size_t &max, size_t &start) const
    {
        if(start >= len_)
        {
            start -= len_;  return;
        }
        size_t len = len_ - start;
        if(len < max)
        {
            memcpy(buf, ptr_ + start, len * sizeof(C));  max -= len;
        }
        else
        {
            memcpy(buf, ptr_ + start, max * sizeof(C));  max = 0;
        }
        start = 0;  buf += len;
    }
};

template<> inline LiteralBase<char>::LiteralBase(const char *str) :
    ptr_(str), len_(str ? std::strlen(str) : 0)
{
}

template<> inline int LiteralBase<char>::cmp(const LiteralBase &str) const
{
    if(!ptr_)return str.ptr_ ? -1 : 0;  if(!str.ptr_)return 1;
    if(len_ < str.len_)return std::memcmp(ptr_, str.ptr_, len_) > 0 ? 1 : -1;
    if(len_ > str.len_)return std::memcmp(ptr_, str.ptr_, str.len_) < 0 ? -1 : 1;
    return std::memcmp(ptr_, str.ptr_, len_);
}

typedef LiteralBase<char> Literal;


template<typename C> class StringBase : public StringLike<StringBase<C>, C>, public Comparable<StringBase<C>, LiteralBase<C> >
{
    size_t *volatile ptr_;  // refcount, length, data


    static size_t *alloc_(size_t n)  // n = len + 1
    {
        if(!n)return 0;
        size_t *ptr_ = reinterpret_cast<size_t *>(new_nt char[2 * sizeof(size_t) + n * sizeof(C)]);
        if(!ptr_)return 0;  ptr_[0] = 0;  ptr_[1] = n - 1;  return ptr_;
    }

    static size_t *copy_(const C *str, size_t n)  // n = len
    {
        if(!str)return 0;
        size_t *ptr_ = reinterpret_cast<size_t *>(new_nt char[2 * sizeof(size_t) + (n + 1) * sizeof(C)]);
        if(!ptr_)return 0;  ptr_[0] = 0;  ptr_[1] = n;  memcpy(ptr_ + 2, str, n * sizeof(C));
        reinterpret_cast<C *>(ptr_ + 2)[n] = C(0);  return ptr_;
    }

    void add_ref_() const
    {
        if(ptr_)sync_inc(ptr_[0]);
    }

    void free_()
    {
        if(ptr_ && !sync_dec(ptr_[0]))delete [] ptr_;
    }


protected:
    explicit StringBase(size_t n)
    {
        ptr_ = alloc_(n);
    }

    bool resize(size_t n)
    {
        free_();  return (ptr_ = alloc_(n)) != 0;
    }

    const C *buffer() const
    {
        assert(ptr_ && !ptr_[0]);  return reinterpret_cast<const C *>(ptr_ + 2);
    }

    C *buffer()
    {
        assert(ptr_ && !ptr_[0]);  return reinterpret_cast<C *>(ptr_ + 2);
    }


public:
    StringBase() : ptr_(0)
    {
    }

    StringBase(const StringBase &str)
    {
        str.add_ref_();  ptr_ = str.ptr_;
    }

    StringBase(const LiteralBase<C> &str)
    {
        ptr_ = copy_(str.data(), str.length());
    }

    StringBase(const C *str, size_t n)
    {
        ptr_ = copy_(str, n);
    }

    StringBase(const C *str)
    {
        ptr_ = copy_(str, LiteralBase<C>(str).length());
    }

    template<class S> StringBase(const StringLike<S, C> &str)
    {
        if(!str.valid())
        {
            ptr_ = 0;  return;
        }
        if(!(ptr_ = alloc_(str.length() + 1)))return;  C *buf = buffer();  str.fill(buf);  *buf = C(0);
    }

    ~StringBase()
    {
        free_();
    }


    void clear()
    {
        free_();  ptr_ = 0;
    }

    friend void swap(StringBase<C> &str1, StringBase<C> &str2)
    {
        size_t *tmp = str1.ptr_;  str1.ptr_ = str2.ptr_;  str2.ptr_ = tmp;
    }

    StringBase &operator = (const StringBase &str)
    {
        str.add_ref_();  free_();  ptr_ = str.ptr_;  return *this;
    }

    StringBase &operator = (const LiteralBase<C> &str)
    {
        size_t *res = copy_(str.data(), str.length());  free_();  ptr_ = res;  return *this;
    }

    StringBase &operator = (const C *str)
    {
        return *this = LiteralBase<C>(str);
    }

    template<class S> StringBase &operator = (const StringLike<S, C> &str)
    {
        if(!str.valid())
        {
            clear();  return *this;
        }
        size_t *res = alloc_(str.length() + 1);
        if(res)
        {
            C *buf = reinterpret_cast<C *>(res + 2);  str.fill(buf);  *buf = C(0);
        }
        free_();  ptr_ = res;  return *this;
    }

    template<class S> StringBase &operator += (const S &str)
    {
        return *this = *this + str;
    }

    operator const LiteralBase<C>() const
    {
        return !ptr_ ? LiteralBase<C>() :
            LiteralBase<C>(buffer(), ptr_[1]);
    }


    C operator [] (size_t index) const
    {
        assert(ptr_ && index < ptr_[1]);  return buffer()[index];
    }

    LiteralBase<C> substr(size_t beg, size_t n = size_t(-1)) const
    {
        if(!ptr_)return LiteralBase<C>();
        if(beg > ptr_[1])beg = ptr_[1];  if(n > ptr_[1] - beg)n = ptr_[1] - beg;
        return LiteralBase<C>(buffer() + beg, n);
    }

    size_t scan(C ch, size_t pos = 0) const
    {
        return LiteralBase<C>(*this).scan(ch, pos);
    }

    size_t rscan(C ch, size_t pos) const
    {
        return LiteralBase<C>(*this).scan(ch, pos);
    }

    size_t rscan(C ch) const
    {
        return LiteralBase<C>(*this).scan(ch);
    }

    int cmp(const LiteralBase<C> &str) const
    {
        return LiteralBase<C>(*this).cmp(str);
    }


    bool valid() const
    {
        return ptr_;
    }

    const C *data() const
    {
        return ptr_ ? buffer() : 0;
    }

    size_t length() const
    {
        return ptr_ ? ptr_[1] : 0;
    }

    void fill(C *&buf) const
    {
        if(!ptr_)return;  memcpy(buf, buffer(), ptr_[1] * sizeof(C));  buf += ptr_[1];
    }

    void fill(C *&buf, size_t &max, size_t &start) const
    {
        if(!ptr_)return;
        if(start >= ptr_[1])
        {
            start -= ptr_[1];  return;
        }
        size_t len = ptr_[1] - start;
        if(len < max)
        {
            memcpy(buf, buffer() + start, len * sizeof(C));  max -= len;
        }
        else
        {
            memcpy(buf, buffer() + start, max * sizeof(C));  max = 0;
        }
        start = 0;  buf += len;
    }
};

typedef StringBase<char> String;
