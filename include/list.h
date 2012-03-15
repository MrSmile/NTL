/***************************************************************************************************
* Nothrow Template Library : list.h : list containers
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

#include "allocator.h"



namespace NTL_Internal_ {



template<typename T> class StackNode : public Heavy
{
    template<typename, typename> friend class Stack;


    StackNode<T> *next_;


public:
#ifdef NTL_DEBUG
    StackNode() : next_(invalid_ptr)
    {
    }

    ~StackNode()
    {
        assert(next_ == invalid_ptr);
    }
#endif

    const T *next() const
    {
        return static_cast<const T *>(next_);
    }

    T *next()
    {
        return static_cast<T *>(next_);
    }

    void insert_next(T *node)
    {
        StackNode<T> *ptr = node;
#ifdef NTL_DEBUG
        assert(next_ != invalid_ptr && ptr->next_ == invalid_ptr);
#endif
        ptr->next_ = next_;  next_ = ptr;
    }

    T *take_next()
    {
        StackNode<T> *node = next_;  if(!node)return 0;  next_ = node->next_;
#ifdef NTL_DEBUG
        node->next_ = invalid_ptr;
#endif
        return static_cast<T *>(node);
    }
};


template<typename T, typename A = EmptyAllocator<T> > class Stack : private A, public Heavy
{
    template<typename, typename> friend class Stack;


    StackNode<T> *first_;


public:
    Stack() : first_(0)
    {
    }

    explicit Stack(const A &alloc) : A(alloc), first_(0)
    {
    }

    ~Stack()
    {
        clear();
    }


    void clear()
    {
        for(StackNode<T> *node = first_; node;)
        {
            StackNode<T> *ptr = node;  node = node->next_;
#ifdef NTL_DEBUG
            ptr->next_ = invalid_ptr;
#endif
            remove(static_cast<T *>(ptr));
        }
        first_ = 0;
    }

    template<typename A1> bool copy(const Stack<T, A1> &stack)
    {
        assert(!first_);  StackNode<T> **last = &first_;
        for(StackNode<T> *node = stack.first_; node; node = node->next_)
        {
            StackNode<T> *ptr = create(*static_cast<T *>(node));
            if(!ptr)
            {
                *last = 0;  clear();  return false;
            }
            *last = ptr;  last = &ptr->next_;
        }
        *last = 0;  return true;
    }

    friend void swap(Stack<T, A> &stack1, Stack<T, A> &stack2)
    {
        NTL::swap(stack1.first_, stack2.first_);
    }


    const T *first() const
    {
        return static_cast<const T *>(first_);
    }

    T *first()
    {
        return static_cast<T *>(first_);
    }

    T *take_first()
    {
        StackNode<T> *node = first_;  if(!node)return 0;  first_ = node->next_;
#ifdef NTL_DEBUG
        node->next_ = invalid_ptr;
#endif
        return static_cast<T *>(node);
    }

    void prepend(T *node)
    {
        StackNode<T> *ptr = node;
#ifdef NTL_DEBUG
        assert(ptr->next_ == invalid_ptr);
#endif
        ptr->next_ = first_;  first_ = ptr;
    }


    bool operator ! () const
    {
        return first_ == 0;
    }

    bool not_empty() const
    {
        return first_ != 0;
    }

    operator bool() const
    {
        return first_ != 0;
    }
};


template<typename T> class OwningStack : public Stack<T, DefaultAllocator<T> >  // template typedef workaround
{
    typedef Stack<T, DefaultAllocator<T> > Base_;

    friend void swap(OwningStack<T> &stack1, OwningStack<T> &stack2)
    {
        swap(static_cast<Base_ &>(stack1), static_cast<Base_ &>(stack2));
    }
};



template<typename T> class SimpleListNode : public Heavy
{
    template<typename, typename> friend class SimpleList;


    SimpleListNode<T> *next_;


public:
#ifdef NTL_DEBUG
    SimpleListNode() : next_(invalid_ptr)
    {
    }

    ~SimpleListNode()
    {
        assert(next_ == invalid_ptr);
    }
#endif

    const T *next() const
    {
        return static_cast<const T *>(next_);
    }

    T *next()
    {
        return static_cast<T *>(next_);
    }
};


template<typename T, typename A = EmptyAllocator<T> > class SimpleList : private A, public Heavy
{
    SimpleListNode<T> *first_, **last_;


public:
    SimpleList() : first_(0), last_(&first_)
    {
    }

    explicit SimpleList(const A &alloc) : A(alloc), first_(0), last_(&first_)
    {
    }

    ~SimpleList()
    {
        clear();
    }


    void clear()
    {
        for(SimpleListNode<T> *node = first_; node;)
        {
            SimpleListNode<T> *ptr = node;  node = node->next_;
#ifdef NTL_DEBUG
            ptr->next_ = invalid_ptr;
#endif
            remove(static_cast<T *>(ptr));
        }
        first_ = 0;  last_ = &first_;
    }

    template<typename A1> bool copy(const SimpleList<T, A1> &list)
    {
        assert(!first_);
        for(SimpleListNode<T> *node = list.first_; node; node = node->next_)
        {
            SimpleListNode<T> *ptr = create(*static_cast<T *>(node));
            if(!ptr)
            {
                *last_ = 0;  clear();  return false;
            }
            *last_ = ptr;  last_ = &ptr->next_;
        }
        *last_ = 0;  return true;
    }

    void prepend(SimpleList<T, A> &list)
    {
        if(!list.first_)return;
        if(first_)*list.last_ = first_;  else last_ = list.last_;
        first_ = list.first_;  list.first_ = 0;  list.last_ = &list.first_;
    }

    void append(SimpleList<T, A> &list)
    {
        if(!list.first_)return;
        *last_ = list.first_;  last_ = list.last_;
        list.first_ = 0;  list.last_ = &list.first_;
    }

    template<typename A1> bool prepend_copy(const SimpleList<T, A1> &list)
    {
        SimpleList<T, A> clone;  if(!clone.copy(list))return false;
        prepend(clone);  return true;
    }

    template<typename A1> bool append_copy(const SimpleList<T, A1> &list)
    {
        SimpleList<T, A> clone;  if(!clone.copy(list))return false;
        append(clone);  return true;
    }

    friend void swap(SimpleList<T, A> &list1, SimpleList<T, A> &list2)
    {
        NTL::swap(list1.first_, list2.first_);  NTL::swap(list1.last_, list2.last_);
        if(!list1.first_)list1.last_ = &list1.first_;
        if(!list2.first_)list2.last_ = &list2.first_;
    }


    const T *first() const
    {
        return static_cast<const T *>(first_);
    }

    T *first()
    {
        return static_cast<T *>(first_);
    }

    T *take_first()
    {
        SimpleListNode<T> *node = first_;  if(!node)return 0;
        if(!(first_ = node->next_))last_ = &first_;
#ifdef NTL_DEBUG
        node->next_ = invalid_ptr;
#endif
        return static_cast<T *>(node);
    }

    T *take(T *after)
    {
        SimpleListNode<T> *prev = after, *node = prev->next_;  if(!node)return 0;
        if(!(prev->next_ = node->next_))
        {
            assert(last_ == &node->next_);  last_ = &prev->next_;
        }
#ifdef NTL_DEBUG
        node->next_ = invalid_ptr;
#endif
        return static_cast<T *>(node);
    }


    void insert(T *node, T *after)
    {
        SimpleListNode<T> *ptr = node, *prev = after;
#ifdef NTL_DEBUG
        assert(prev->next_ != invalid_ptr && ptr->next_ == invalid_ptr);
#endif
        if(!(ptr->next_ = prev->next_))
        {
            assert(last_ == &prev->next_);  last_ = &ptr->next_;
        }
        prev->next_ = ptr;
    }

    void prepend(T *node)
    {
        SimpleListNode<T> *ptr = node;
#ifdef NTL_DEBUG
        assert(ptr->next_ == invalid_ptr);
#endif
        if(!first_)last_ = &ptr->next_;  ptr->next_ = first_;  first_ = ptr;
    }

    void append(T *node)
    {
        SimpleListNode<T> *ptr = node;
#ifdef NTL_DEBUG
        assert(ptr->next_ == invalid_ptr);
#endif
        *last_ = ptr;  last_ = &ptr->next_;  ptr->next_ = 0;
    }


    bool operator ! () const
    {
        return first_ == 0;
    }

    bool not_empty() const
    {
        return first_ != 0;
    }

    operator bool() const
    {
        return first_ != 0;
    }
};


template<typename T> class OwningSimpleList : public SimpleList<T, DefaultAllocator<T> >  // template typedef workaround
{
    typedef SimpleList<T, DefaultAllocator<T> > Base_;

    friend void swap(OwningSimpleList<T> &list1, OwningSimpleList<T> &list2)
    {
        swap(static_cast<Base_ &>(list1), static_cast<Base_ &>(list2));
    }
};



template<typename T> class ListNode : public Heavy
{
    template<typename, typename> friend class List;


    ListNode<T> *next_, **prev_;


public:
    ListNode() : prev_(0)
    {
    }

    ~ListNode()
    {
        if(prev_)remove();
    }


    const T *next() const
    {
        assert(prev_);  return static_cast<const T *>(next_);
    }

    T *next()
    {
        assert(prev_);  return static_cast<T *>(next_);
    }


    void remove()
    {
        assert(prev_);  if(next_)next_->prev_ = prev_;
        *prev_ = next_;  next_ = 0;  prev_ = 0;
    }

    T *take_next()
    {
        ListNode<T> *node = next_;  if(!node)return 0;
        if((next_ = node->next_))next_->prev_ = &next_;
        node->prev_ = 0;  return static_cast<T *>(node);
    }

    void insert_prev(T *node)
    {
        ListNode<T> *ptr = node;  assert(prev_ && !ptr->prev_);
        ptr->prev_ = prev_;  ptr->next_ = this;
        *prev_ = ptr;  prev_ = &ptr->next_;
    }

    void insert_next(T *node)
    {
        ListNode<T> *ptr = node;  assert(prev_ && !ptr->prev_);
        if(next_)next_->prev_ = &ptr->next_;  ptr->next_ = next_;
        ptr->prev_ = &next_;  next_ = ptr;
    }


    bool assigned() const
    {
        return prev_ != 0;
    }
};


template<typename T, typename A = EmptyAllocator<T> > class List : private A, public Heavy
{
    ListNode<T> *first_;


    void swap_(List<T, A> &list)
    {
        NTL::swap(list.first_, first_);
        if(list.first_)list.first_->prev_ = &list.first_;
        if(first_)first_->prev_ = &first_;
    }

public:
    List() : first_(0)
    {
    }

    explicit List(const A &alloc) : A(alloc), first_(0)
    {
    }

    ~List()
    {
        clear();
    }


    void clear()
    {
        for(ListNode<T> *node = first_; node;)
        {
            node->prev_ = 0;  T *ptr = static_cast<T *>(node);
            node = node->next_;  remove(ptr);
        }
        first_ = 0;
    }

    template<typename A1> bool copy(const List<T, A1> &list)
    {
        assert(!first_);  ListNode<T> **last = &first_;
        for(ListNode<T> *node = list.first_; node; node = node->next_)
        {
            ListNode<T> *ptr = create(*static_cast<T *>(node));
            if(!ptr)
            {
                *last = 0;  clear();  return false;
            }
            ptr->prev_ = last;  *last = ptr;  last = &ptr->next_;
        }
        *last = 0;  return true;
    }

    friend void swap(List<T, A> &list1, List<T, A> &list2)
    {
        list1.swap_(list2);
    }


    const T *first() const
    {
        return static_cast<const T *>(first_);
    }

    T *first()
    {
        return static_cast<T *>(first_);
    }

    T *take_first()
    {
        ListNode<T> *node = first_;  if(!node)return 0;
        if((first_ = node->next_))first_->prev_ = &first_;
        node->prev_ = 0;  return static_cast<T *>(node);
    }

    void prepend(T *node)
    {
        ListNode<T> *ptr = node;  assert(!ptr->prev_);
        if(first_)first_->prev_ = &ptr->next_;
        ptr->next_ = first_;  ptr->prev_ = &first_;  first_ = ptr;
    }


    bool operator ! () const
    {
        return first_ == 0;
    }

    bool not_empty() const
    {
        return first_ != 0;
    }

    operator bool() const
    {
        return first_ != 0;
    }
};


template<typename T> class OwningList : public List<T, DefaultAllocator<T> >  // template typedef workaround
{
    typedef List<T, DefaultAllocator<T> > Base_;

    friend void swap(OwningList<T> &list1, OwningList<T> &list2)
    {
        swap(static_cast<Base_ &>(list1), static_cast<Base_ &>(list2));
    }
};



}  // end namespace NTL_Internal_



namespace NTL
{
    using NTL_Internal_::StackNode;
    using NTL_Internal_::Stack;
    using NTL_Internal_::OwningStack;

    using NTL_Internal_::SimpleListNode;
    using NTL_Internal_::SimpleList;
    using NTL_Internal_::OwningSimpleList;

    using NTL_Internal_::ListNode;
    using NTL_Internal_::List;
    using NTL_Internal_::OwningList;
}
