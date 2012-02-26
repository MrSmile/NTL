/***************************************************************************************************
* Nothrow Template Library : format.h : string formatting
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

#include "literal.h"



namespace NTL_Internal_ {



template<typename S, typename C, C ch> class Argument : public StringLike<Argument<S, C, ch>, C>
{
    S arg_;

public:
    template<typename T1> explicit Argument(const T1 &arg1) : arg_(arg1)
    {
    }

    template<typename T1, typename T2> Argument(const T1 &arg1, const T2 &arg2) : arg_(arg1, arg2)
    {
    }

    template<typename T1, typename T2, typename T3>
        Argument(const T1 &arg1, const T2 &arg2, const T3 &arg3) : arg_(arg1, arg2, arg3)
    {
    }

    bool valid() const
    {
        return arg_.valid();
    }

    size_t length() const
    {
        return arg_.length();
    }

    template<typename B> bool fill(B &buf) const
    {
        return arg_.fill(buf);
    }
};

template<long ch, typename S, typename C> Argument<const S &, C, C(ch)> arg(const StringLike<S, C> &str)
{
    return Argument<const S &, C, C(ch)>(static_cast<const S &>(str));
}

template<long ch, typename C> Argument<LiteralBase<C>, C, C(ch)> arg(const C *str, size_t n)
{
    return Argument<LiteralBase<C>, C, C(ch)>(LiteralBase<C>(str, n));
}

template<long ch, typename C> Argument<LiteralBase<C>, C, C(ch)> arg(const C *str)
{
    return Argument<LiteralBase<C>, C, C(ch)>(str);
}

template<typename S, typename C> Argument<const S &, C, C(0)> arg(const StringLike<S, C> &str)
{
    return Argument<const S &, C, C(0)>(static_cast<const S &>(str));
}

template<typename C> Argument<LiteralBase<C>, C, C(0)> arg(const C *str, size_t n)
{
    return Argument<LiteralBase<C>, C, C(0)>(LiteralBase<C>(str, n));
}

template<typename C> Argument<LiteralBase<C>, C, C(0)> arg(const C *str)
{
    return Argument<LiteralBase<C>, C, C(0)>(str);
}



template<typename T, typename S, typename C, C ch, C def> class Composite;

template<typename T, typename C, C def> class ArgBase : public StringLike<T, C>
{
    enum Next_
    {
        next_ = def < C('9') ? def + 1 : C(0)
    };


public:
    template<typename S, C ch> Composite<T, const Argument<S, C, ch> &, C, ch, next_>
        operator % (const Argument<S, C, ch> &str) const
    {
        return Composite<T, const Argument<S, C, ch> &, C, ch, next_>(*static_cast<const T *>(this), str);
    }

    template<typename S> Composite<T, const Argument<S, C, C(0)> &, C, def, next_>
        operator % (const Argument<S, C, C(0)> &str) const
    {
        return Composite<T, const Argument<S, C, C(0)> &, C, def, next_>(*static_cast<const T *>(this), str);
    }

    template<typename S> Composite<T, const StringLike<S, C> &, C, def, next_>
        operator % (const StringLike<S, C> &str) const
    {
        return Composite<T, const StringLike<S, C> &, C, def, next_>(*static_cast<const T *>(this), str);
    }

    Composite<T, LiteralBase<C>, C, def, next_> operator % (const C *str) const
    {
        return Composite<T, LiteralBase<C>, C, def, next_>(*static_cast<const T *>(this), str);
    }

    size_t length() const;
    template<typename B> bool fill(B &buf) const;
};


template<typename T, typename S, typename C, C ch, C def> class Composite :
    public ArgBase<Composite<T, S, C, ch, def>, C, def>
{
    template<typename, typename C1, C1> friend class ArgBase;
    template<typename, typename, typename C1, C1, C1> friend class Composite;
    template<typename, typename> friend class FormatMaker;

    enum Flag_
    {
        flag_ = T::flag_
    };


    const T &prev_;
    S arg_;


public:
    template<typename S1> Composite(const T &prev, const S1 &arg) : prev_(prev), arg_(arg)
    {
    }

    bool valid() const
    {
        return prev_.valid() && arg_.valid();
    }
};


template<typename C, C ch> class Format : public ArgBase<Format<C, ch>, C, C('1')>
{
    template<typename, typename C1, C1> friend class ArgBase;
    template<typename, typename, typename C1, C1, C1> friend class Composite;
    template<typename, typename> friend class FormatMaker;

    enum Flag_
    {
        flag_ = ch
    };


    LiteralBase<C> fmt_;


public:
    Format(const LiteralBase<C> &str) : fmt_(str)
    {
    }

    bool valid() const
    {
        return fmt_.valid();
    }
};

template<long ch, typename C> Format<C, C(ch)> format(const LiteralBase<C> &str)
{
    return Format<C, C(ch)>(str);
}

template<long ch, typename C> Format<C, C(ch)> format(const C *str, size_t n)
{
    return Format<C, C(ch)>(LiteralBase<C>(str, n));
}

template<long ch, typename C> Format<C, C(ch)> format(const C *str)
{
    return Format<C, C(ch)>(LiteralBase<C>(str));
}

template<typename C> Format<C, C('%')> format(const LiteralBase<C> &str)
{
    return Format<C, C('%')>(str);
}

template<typename C> Format<C, C('%')> format(const C *str, size_t n)
{
    return Format<C, C('%')>(LiteralBase<C>(str, n));
}

template<typename C> Format<C, C('%')> format(const C *str)
{
    return Format<C, C('%')>(LiteralBase<C>(str));
}



template<typename T, typename C> struct FormatMaker
{
    static const LiteralBase<C> &format(const T &obj)
    {
        return obj.fmt_;
    }

    static size_t length(const T &obj, C cur)
    {
        return 1;
    }

    template<typename B> static bool fill(const T &obj, C cur, B &buf)
    {
        return buf(cur);
    }
};

template<typename T, typename S, typename C, C ch, C def> struct FormatMaker<Composite<T, S, C, ch, def>, C>
{
    static const LiteralBase<C> &format(const Composite<T, S, C, ch, def> &obj)
    {
        return FormatMaker<T, C>::format(obj.prev_);
    }

    static size_t length(const Composite<T, S, C, ch, def> &obj, C cur)
    {
        return cur == ch ? obj.arg_.length() : FormatMaker<T, C>::length(obj.prev_, cur);
    }

    template<typename B> static bool fill(const Composite<T, S, C, ch, def> &obj, C cur, B &buf)
    {
        return cur == ch ? obj.arg_.fill(buf) : FormatMaker<T, C>::fill(obj.prev_, cur, buf);
    }
};


template<typename T, typename C, C def> size_t ArgBase<T, C, def>::length() const
{
    const T &self = *static_cast<const T *>(this);
    const LiteralBase<C> &fmt = FormatMaker<T, C>::format(self);  size_t res = fmt.length();
    for(size_t i = 0; i < fmt.length(); i++)if(fmt[i] == C(T::flag_))
    {
        res -= 2;  if(++i >= fmt.length())return res + 1;
        res += FormatMaker<T, C>::length(self, fmt[i]);
    }
    return res;
}

template<typename T, typename C, C def> template<typename B> bool ArgBase<T, C, def>::fill(B &buf) const
{
    const T &self = *static_cast<const T *>(this);
    const LiteralBase<C> &fmt = FormatMaker<T, C>::format(self);
    for(size_t i = 0; i < fmt.length(); i++)
    {
        if(fmt[i] == C(T::flag_))
        {
            if(++i >= fmt.length())return true;
            if(!FormatMaker<T, C>::fill(self, fmt[i], buf))return false;
        }
        else if(!buf(fmt[i]))return false;
    }
    return true;
}



}  // end namespace NTL_Internal_



namespace NTL
{
    using NTL_Internal_::format;
    using NTL_Internal_::arg;
}
