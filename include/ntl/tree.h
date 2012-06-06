/***************************************************************************************************
* Nothrow Template Library : tree.h : AA tree classes
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



namespace NTL_ {



template<typename H> class GeneralTreeNode : public H::NodeBase, public Heavy
{
    template<typename> friend class GeneralTreeBase;
    template<typename, typename, typename> friend class GeneralTree;
    friend class IndexerNodeBase;
    friend class IndexerBase;


    typedef GeneralTreeNode<H> Node_;
    typedef typename H::Tree Tree_;


    enum NodeType_
    {
        t_root = 0, t_right, t_left, t_left_red
    };


    Node_ *parent_, *left_, *right_;
    NodeType_ type_;


    const Node_ *first_() const
    {
        for(const Node_ *node = this;; node = node->left_)if(!node->left_)return node;
    }

    const Node_ *last_() const
    {
        for(const Node_ *node = this;; node = node->right_)if(!node->right_)return node;
    }


    void set_parent_(Node_ *ptr, NodeType_ tp)
    {
        parent_ = ptr;
        switch(type_ = tp)
        {
        case t_root:  reinterpret_cast<Tree_ *>(ptr)->root_ = this;  return;
        case t_right:  ptr->right_ = this;  return;
        default:  ptr->left_ = this;  return;
        }
    }

    void rotate_left_()
    {
        Node_ *next = right_;  next->set_parent_(parent_, type_);
        if((right_ = next->left_))
        {
            right_->parent_ = this;  right_->type_ = t_right;
        }
        next->left_ = this;  parent_ = next;  type_ = t_left_red;  next->add_index_(this);
    }

    void rotate_right_()
    {
        Node_ *next = left_;  next->set_parent_(parent_, type_);
        if((left_ = next->right_))
        {
            left_->parent_ = this;  left_->type_ = t_left;
        }
        next->right_ = this;  parent_ = next;  type_ = t_right;  sub_index_(next);
    }

    Node_ *make_red_()
    {
        switch(type_)
        {
        case t_right:
            if(parent_->left_ && parent_->left_->type_ == t_left_red)
            {
                parent_->left_->type_ = t_left;  return parent_;
            }
            parent_->rotate_left_();  if(type_ != t_left_red)return 0;
            left_->type_ = t_left;  parent_->rotate_right_();  return this;

        case t_left:
            if(parent_->type_ == t_left_red)
            {
                parent_->parent_->rotate_right_();  return parent_;
            }
            type_ = t_left_red;
        default:
            return 0;
        }
    }

    Node_ *remove_red_()
    {
        switch(type_)
        {
        case t_right:
            if(parent_->left_->type_ == t_left)
            {
                if(!parent_->left_->left_ || parent_->left_->left_->type_ != t_left_red)
                {
                    parent_->left_->type_ = t_left_red;  return parent_;
                }
                parent_->left_->left_->type_ = t_left;  parent_->rotate_right_();
            }
            else if(parent_->left_->right_->left_ && parent_->left_->right_->left_->type_ == t_left_red)
            {
                parent_->left_->rotate_left_();  parent_->rotate_right_();
            }
            else
            {
                parent_->rotate_right_();  parent_->left_->type_ = t_left_red;
            }
            return 0;

        case t_left:
            if(!parent_->right_->left_ || parent_->right_->left_->type_ != t_left_red)
            {
                parent_->rotate_left_();  return parent_->parent_;
            }
            parent_->right_->rotate_right_();  parent_->rotate_left_();
            parent_->type_ = t_left;  return 0;

        case t_left_red:
            type_ = t_left;
        default:
            return 0;
        }
    }

    void insert_(Node_ *node, bool after)
    {
        assert(parent_ && !node->parent_);  node->parent_ = this;  node->left_ = node->right_ = 0;
        if(after)
        {
            assert(!right_);  right_ = node;  node->type_ = t_right;
        }
        else
        {
            assert(!left_);  left_ = node;  node->type_ = t_left;
        }
        node->inc_index_();
        do node = node->make_red_();
        while(node);
    }

#ifdef NTL_DEBUG
    size_t check_() const
    {
        size_t lev = 0;
        assert(type_ != t_left_red || parent_->type_ != t_left_red);
        if(left_)
        {
            assert(left_->parent_ == this && left_->type_ > t_right);
            lev = left_->check_();
            if(right_)
            {
                assert(right_->parent_ == this && right_->type_ == t_right && left_);
                assert(lev == right_->check_());
            }
            else assert(left_->type_ == t_left_red && !left_->left_ && !left_->right_);
        }
        else assert(!right_);
        assert(type_ != t_root || reinterpret_cast<Tree_ *>(parent_)->root_ == this);
        return type_ == t_left_red ? lev : lev + 1;
    }
#endif


    struct Place_
    {
        const Node_ *node;
        int dir;

        Place_() : node(0), dir(-1)
        {
        }

        Place_(const Node_ *ptr, int d)
        {
            if(d > 0)
                if(ptr->right_)
                {
                    node = ptr->right_->first_();  dir = -1;
                }
                else
                {
                    node = ptr;  dir = 1;
                }
            else if(d < 0)
                if(ptr->left_)
                {
                    node = ptr->left_->last_();  dir = 1;
                }
                else
                {
                    node = ptr;  dir = -1;
                }
            else
            {
                node = ptr;  dir = 0;
            }
        }
    };


public:
    GeneralTreeNode() : parent_(0)
    {
    }

    ~GeneralTreeNode()
    {
        if(parent_)remove();
    }

    void remove()
    {
        assert(parent_);
        if(left_)
        {
            Node_ *node = const_cast<Node_ *>(left_->last_());
            Node_ *leaf = node->left_, *np = parent_;  NodeType_ nt = type_;
            if(leaf)
            {
                leaf->set_parent_(node->parent_, node->type_);  node->set_parent_(np, nt);
                node->left_ = left_;  node->right_ = right_;  left_->parent_ = right_->parent_ = node;
                node->copy_index_(this);  leaf->dec_index_();  parent_ = 0;  return;
            }
            if(node != left_)
            {
                set_parent_(node->parent_, t_right);
                node->left_ = left_;  left_->parent_ = node;
            }
            else if(node->type_ == t_left_red)
            {
                node->set_parent_(np, nt);  node->right_ = 0;
                node->dec_index_();  parent_ = 0;  return;
            }
            else
            {
                parent_ = node;  type_ = node->type_;
                node->left_ = this;
            }
            if((node->right_ = right_))right_->parent_ = node;
            node->set_parent_(np, nt);  left_ = right_ = 0;  swap_index_(node);
        }

        Node_ *node = this;
        do node = node->remove_red_();
        while(node);

        H::NodeBase::dec_index_();
        if(type_ > t_right)parent_->left_ = 0;
        else if(type_ == t_right)parent_->right_ = 0;
        else reinterpret_cast<Tree_ *>(parent_)->root_ = 0;
        parent_ = 0;
    }

    const Node_ *prev() const
    {
        assert(parent_);  const Node_ *node = this;
        if(!left_)
        {
            while(node->type_ > t_right)node = node->parent_;
            return node->type_ ? node->parent_ : 0;
        }
        for(node = node->left_;; node = node->right_)
            if(!node->right_)return node;
    }

    const Node_ *next() const
    {
        assert(parent_);  const Node_ *node = this;
        if(!right_)
        {
            while(node->type_ == t_right)node = node->parent_;
            return node->type_ ? node->parent_ : 0;
        }
        for(node = node->right_;; node = node->left_)
            if(!node->left_)return node;
    }

    bool assigned()
    {
        return parent_ != 0;
    }
};


template<typename H> class GeneralTreeBase : public H::Base
{
    template<typename> friend class GeneralTreeNode;
    template<typename, typename, typename> friend class GeneralTree;
    friend class IndexerBase;


    typedef typename H::Node Node_;


    Node_ *root_;


    Node_ *root_cast_()
    {
        return reinterpret_cast<Node_ *>(this);
    }

    void swap_(GeneralTreeBase<H> &tree)
    {
        if(root_)root_->parent_ = tree.root_cast_();
        if(tree.root_)tree.root_->parent_ = root_cast_();
        swap(root_, tree.root_);  swap_size_(tree);
    }

public:
    GeneralTreeBase() : root_(0)
    {
    }

    size_t check_() const
    {
#ifdef NTL_DEBUG
        if(!root_)return 0;  assert(root_->type_ == Node_::t_root);  return root_->check_();
#else
        return 0;
#endif
    }


    bool operator ! () const
    {
        return root_ == 0;
    }

    bool not_empty() const
    {
        return root_ != 0;
    }

    operator bool() const
    {
        return root_ != 0;
    }
};


template<typename N, typename H, typename A> class GeneralTree : private A, public H::Tree, public Heavy
{
    template<typename, typename> friend class Indexer;

    typedef typename H::Tree Tree_;
    typedef typename H::Node Node_;
    typedef typename N::Type_ Type_;
    typedef typename Node_::Place_ PlaceBase_;


    static const Type_ *const_cast_(const Node_ *node)
    {
        return static_cast<const Type_ *>(static_cast<const N *>(node));
    }

    static Type_ *cast_(const Node_ *node)
    {
        return const_cast<Type_ *>(static_cast<const Type_ *>(static_cast<const N *>(node)));
    }

    void set_root_(Node_ *node)
    {
        assert(!Tree_::root_);  Tree_::root_ = node;  node->parent_ = Tree_::root_cast_();
        node->left_ = node->right_ = 0;  node->type_ = Node_::t_root;
        node->inc_index_();
    }

    Node_ *copy_node_(Node_ *parent, const Node_ *old)
    {
        N *node = A::create(*const_cast_(old));  if(!node)return 0;
        node->parent_ = parent;  node->left_ = node->right_ = 0;
        node->type_ = old->type_;  node->copy_index_(old);  return node;
    }

    template<typename K> PlaceBase_ find_place_(const K &key) const
    {
        PlaceBase_ res;
        for(const Node_ *node = Tree_::root_; node;)
        {
            int dir = -const_cast_(node)->cmp(key);
            res.node = node;  res.dir = dir;
            if(dir > 0)node = node->right_;
            else if(dir < 0)node = node->left_;
            else return res;
        }
        return res;
    }

    void swap_(GeneralTree<N, H, A> &tree)
    {
        Tree_::swap_(tree);
    }

    void insert_(N *node, const PlaceBase_ &place)
    {
        assert(place.dir);
        if(place.node)const_cast<Node_ *>(place.node)->insert_(node, place.dir > 0);
        else set_root_(node);
    }


public:
    class Place : protected PlaceBase_
    {
        template<typename, typename, typename> friend class GeneralTree;
        template<typename, typename> friend class Indexer;

    protected:
        Place(const PlaceBase_ &place) : PlaceBase_(place)
        {
        }

    public:
        Place(const Place &place) : PlaceBase_(place)
        {
        }

        Place(Type_ *node, bool after) : PlaceBase_(static_cast<N *>(node), after ? 1 : -1)
        {
        }

        Place &operator = (const Place &place)
        {
            *static_cast<PlaceBase_ *>(this) = place;  return *this;
        }

        Type_ *node() const
        {
            return PlaceBase_::dir ? 0 : cast_(PlaceBase_::node);
        }

        Type_ *before() const
        {
            if(!PlaceBase_::node)return 0;
            return cast_(PlaceBase_::dir >= 0 ? PlaceBase_::node : PlaceBase_::node->prev());
        }

        Type_ *after() const
        {
            if(!PlaceBase_::node)return 0;
            return cast_(PlaceBase_::dir <= 0 ? PlaceBase_::node : PlaceBase_::node->next());
        }

        size_t index() const
        {
            if(!PlaceBase_::node)return 0;
            return PlaceBase_::dir > 0 ? PlaceBase_::node->index() + 1 : PlaceBase_::node->index();
        }
    };

    class ConstPlace : protected PlaceBase_
    {
        template<typename, typename, typename> friend class GeneralTree;
        template<typename, typename> friend class Indexer;

    protected:
        ConstPlace(const PlaceBase_ &place) : PlaceBase_(place)
        {
        }

    public:
        ConstPlace(const Place &place) : PlaceBase_(place)
        {
        }

        ConstPlace(const ConstPlace &place) : PlaceBase_(place)
        {
        }

        ConstPlace(const Type_ *node, bool after) : PlaceBase_(static_cast<const N *>(node), after ? 1 : -1)
        {
        }

        ConstPlace &operator = (const Place &place)
        {
            *static_cast<PlaceBase_ *>(this) = place;  return *this;
        }

        ConstPlace &operator = (const ConstPlace &place)
        {
            *static_cast<PlaceBase_ *>(this) = place;  return *this;
        }

        const Type_ *node() const
        {
            return PlaceBase_::dir ? 0 : const_cast_(PlaceBase_::node);
        }

        const Type_ *before() const
        {
            if(!PlaceBase_::node)return 0;
            return const_cast_(PlaceBase_::dir >= 0 ? PlaceBase_::node : PlaceBase_::node->prev());
        }

        const Type_ *after() const
        {
            if(!PlaceBase_::node)return 0;
            return const_cast_(PlaceBase_::dir <= 0 ? PlaceBase_::node : PlaceBase_::node->next());
        }

        size_t index() const
        {
            if(!PlaceBase_::node)return 0;
            return PlaceBase_::dir > 0 ? PlaceBase_::node->index() + 1 : PlaceBase_::node->index();
        }
    };


    GeneralTree()
    {
    }

    explicit GeneralTree(const A &alloc) : A(alloc)
    {
    }

    ~GeneralTree()
    {
        clear();
    }


    void clear()
    {
        if(Tree_::root_)for(Node_ *node = Tree_::root_;;)
        {
            if(node->left_)node = node->left_;
            else if(node->right_)node = node->right_;
            else
            {
                Node_ *ptr = node;  node = node->parent_;
                if(ptr->type_ > Node_::t_right)node->left_ = 0;
                else if(ptr->type_ == Node_::t_right)node->right_ = 0;
                else
                {
                    ptr->parent_ = 0;  A::remove(cast_(ptr));
                    Tree_::root_ = 0;  Tree_::clear_size_();  return;
                }
                ptr->parent_ = 0;  A::remove(cast_(ptr));
            }
        }
    }

    template<typename A1> bool copy(const GeneralTree<N, H, A1> &tree)
    {
        assert(!Tree_::root_);  if(!tree.Tree_::root_)return true;
        Node_ *node = copy_node_(Tree_::root_cast_(), tree.Tree_::root_);
        if(!node)return false;  Tree_::root_ = node;  copy_size_(tree);

        for(const Node_ *old = tree.Tree_::root_;;)
            if(!node->left_ && old->left_)
            {
                if(!(node = node->left_ = copy_node_(node, old = old->left_)))break;
            }
            else if(!node->right_ && old->right_)
            {
                if(!(node = node->right_ = copy_node_(node, old = old->right_)))break;
            }
            else
            {
                if(old->type_ == Node_::t_root)return true;
                old = old->parent_;  node = node->parent_;
            }
        clear();  return false;
    }

    friend void swap(GeneralTree<N, H, A> &tree1, GeneralTree<N, H, A> &tree2)
    {
        tree1.swap_(tree2);
    }


    const Type_ *first() const
    {
        return Tree_::root_ ? const_cast_(Tree_::root_->first_()) : 0;
    }

    Type_ *first()
    {
        return Tree_::root_ ? cast_(Tree_::root_->first_()) : 0;
    }

    const Type_ *last() const
    {
        return Tree_::root_ ? const_cast_(Tree_::root_->last_()) : 0;
    }

    Type_ *last()
    {
        return Tree_::root_ ? cast_(Tree_::root_->last_()) : 0;
    }


    template<typename K> ConstPlace find_place(const K &key) const
    {
        return find_place_(key);
    }

    template<typename K> Place find_place(const K &key)
    {
        return find_place_(key);
    }

    template<typename K> const Type_ *find(const K &key) const
    {
        PlaceBase_ place = find_place_(key);  return place.dir ? 0 : const_cast_(place.node);
    }

    template<typename K> Type_ *find(const K &key)
    {
        return const_cast<Type_ *>(const_cast<const GeneralTree<N, H, A> *>(this)->find(key));
    }

    template<typename K> Type_ *take(const K &key)
    {
        PlaceBase_ place = find_place_(key);  if(place.dir)return 0;
        Node_ *node = const_cast<Node_ *>(place.node);
        node->remove();  return cast_(node);
    }


    void insert(Type_ *node, const ConstPlace &place)
    {
        insert_(node, place);
    }

    void insert(Type_ *node, const Place &place)
    {
        insert_(node, place);
    }

    void insert(Type_ *node)
    {
        insert_(node, find_place_(*node));
    }

    template<typename K> Type_ *get(const K &key)
    {
        PlaceBase_ place = find_place_(key);
        if(!place.dir)return cast_(place.node);
        Type_ *node = A::create(key);  if(node)insert_(node, place);  return node;
    }
};



class TreeBase;
class TreeNodeBase;

struct TreeTypes
{
    typedef TreeBase Base;
    typedef TreeNodeBase NodeBase;
    typedef GeneralTreeBase<TreeTypes> Tree;
    typedef GeneralTreeNode<TreeTypes> Node;
};


class TreeNodeBase
{
    template<typename> friend class GeneralTreeNode;
    template<typename, typename, typename> friend class GeneralTree;


    void swap_index_(TreeNodeBase *)
    {
    }

    void copy_index_(const TreeNodeBase *)
    {
    }

    void add_index_(const TreeNodeBase *)
    {
    }

    void sub_index_(const TreeNodeBase *)
    {
    }

    void inc_index_()
    {
    }

    void dec_index_()
    {
    }
};


class TreeBase
{
    template<typename> friend class GeneralTreeBase;
    template<typename, typename, typename> friend class GeneralTree;


    void clear_size_()
    {
    }

    void swap_size_(TreeBase &)
    {
    }

    void copy_size_(const TreeBase &)
    {
    }
};


template<typename T> class TreeNode : public GeneralTreeNode<TreeTypes>
{
    template<typename, typename, typename> friend class GeneralTree;

    typedef GeneralTreeNode<TreeTypes> Base_;
    typedef T Type_;


public:
    const T *prev() const
    {
        return static_cast<const T *>(static_cast<const TreeNode<T> *>(Base_::prev()));
    }

    T *prev()
    {
        return const_cast<T *>(static_cast<const T *>(static_cast<const TreeNode<T> *>(Base_::prev())));
    }

    const T *next() const
    {
        return static_cast<const T *>(static_cast<const TreeNode<T> *>(Base_::next()));
    }

    T *next()
    {
        return const_cast<T *>(static_cast<const T *>(static_cast<const TreeNode<T> *>(Base_::next())));
    }
};


template<typename T, typename A = EmptyAllocator<T> > class Tree : public GeneralTree<TreeNode<T>, TreeTypes, A>
{
    typedef GeneralTree<TreeNode<T>, TreeTypes, A> Base_;

public:
    Tree()
    {
    }

    explicit Tree(const A &alloc) : Base_(alloc)
    {
    }

    friend void swap(Tree<T, A> &tree1, Tree<T, A> &tree2)
    {
        swap(static_cast<Base_ &>(tree1), static_cast<Base_ &>(tree2));
    }
};


template<typename T> class OwningTree : public Tree<T, DefaultAllocator<T> >  // template typedef workaround
{
    typedef Tree<T, DefaultAllocator<T> > Base_;

    friend void swap(OwningTree<T> &tree1, OwningTree<T> &tree2)
    {
        swap(static_cast<Base_ &>(tree1), static_cast<Base_ &>(tree2));
    }
};



class IndexerBase;
class IndexerNodeBase;

struct IndexerTypes
{
    typedef IndexerBase Base;
    typedef IndexerNodeBase NodeBase;
    typedef GeneralTreeBase<IndexerTypes> Tree;
    typedef GeneralTreeNode<IndexerTypes> Node;
};


class IndexerNodeBase
{
    template<typename> friend class GeneralTreeNode;
    template<typename, typename, typename> friend class GeneralTree;
    friend class IndexerBase;

    typedef IndexerTypes::Tree Tree_;
    typedef IndexerTypes::Node Node_;


    size_t index_;


    void swap_index_(IndexerNodeBase *node)
    {
        swap(index_, node->index_);
    }

    void copy_index_(const IndexerNodeBase *node)
    {
        index_ = node->index_;
    }

    void add_index_(const IndexerNodeBase *node)
    {
        index_ += node->index_ + 1;
    }

    void sub_index_(const IndexerNodeBase *node)
    {
        index_ -= node->index_ + 1;
    }

    void inc_index_();
    void dec_index_();

#ifdef NTL_DEBUG
    size_t check_index_() const
    {
        const Node_ *node = static_cast<const Node_ *>(this);
        size_t left = (node->left_ ? node->left_->check_index_() : 0);
        size_t right = (node->right_ ? node->right_->check_index_() : 0);
        assert(left == index_);  return left + right + 1;
    }
#endif

public:
    size_t index() const
    {
        assert(static_cast<const Node_ *>(this)->parent_);  size_t index = index_;
        for(const Node_ *node = static_cast<const Node_ *>(this); node->type_ != Node_::t_root; node = node->parent_)
            if(node->type_ == Node_::t_right)index += node->parent_->index_ + 1;  return index;
    }
};


class IndexerBase
{
    template<typename> friend class GeneralTreeBase;
    template<typename, typename, typename> friend class GeneralTree;
    template<typename, typename> friend class Indexer;
    friend class IndexerNodeBase;

    typedef IndexerTypes::Tree Tree_;
    typedef IndexerTypes::Node Node_;
    typedef Node_::Place_ PlaceBase_;


    size_t size_;


    void clear_size_()
    {
        size_ = 0;
    }

    void swap_size_(IndexerBase &tree)
    {
        swap(size_, tree.size_);
    }

    void copy_size_(const IndexerBase &tree)
    {
        size_ = tree.size_;
    }


    const Node_ *at_(size_t index) const
    {
        assert(index < size_);
        const Node_ *node = static_cast<const Tree_ *>(this)->root_;
        while(index != node->index_)
            if(index > node->index_)
            {
                index -= node->index_ + 1;  node = node->right_;
            }
            else node = node->left_;  return node;
    }

    PlaceBase_ place_at_(size_t index) const
    {
        Tree_::PlaceBase_ res;
        for(const Node_ *node = static_cast<const Tree_ *>(this)->root_; node;)
            if(index > node->index_)
            {
                res.node = node;  res.dir = 1;
                index -= node->index_ + 1;  node = node->right_;
            }
            else
            {
                res.node = node;  res.dir = -1;
                node = node->left_;
            }
        return res;
    }

#ifdef NTL_DEBUG
    void check_index_() const
    {
        const Node_ *root = static_cast<const Tree_ *>(this)->root_;
        assert((root ? root->check_index_() : 0) == size_);
    }
#endif

public:
    IndexerBase() : size_(0)
    {
    }

    size_t size() const
    {
        return size_;
    }
};

inline void IndexerNodeBase::inc_index_()
{
    index_ = 0;
    for(Node_ *node = static_cast<Node_ *>(this);; node = node->parent_)
        if(node->type_ == Node_::t_root)
        {
            reinterpret_cast<Tree_ *>(node->parent_)->size_++;  return;
        }
        else if(node->type_ > Node_::t_right)node->parent_->index_++;
}

inline void IndexerNodeBase::dec_index_()
{
    for(Node_ *node = static_cast<Node_ *>(this);; node = node->parent_)
        if(node->type_ == Node_::t_root)
        {
            reinterpret_cast<Tree_ *>(node->parent_)->size_--;  return;
        }
        else if(node->type_ > Node_::t_right)node->parent_->index_--;
}


template<typename T> class IndexerNode : public GeneralTreeNode<IndexerTypes>
{
    template<typename, typename, typename> friend class GeneralTree;

    typedef GeneralTreeNode<IndexerTypes> Base_;
    typedef T Type_;


public:
    const T *prev() const
    {
        return static_cast<const T *>(static_cast<const IndexerNode<T> *>(Base_::prev()));
    }

    T *prev()
    {
        return const_cast<T *>(static_cast<const T *>(static_cast<const IndexerNode<T> *>(Base_::prev())));
    }

    const T *next() const
    {
        return static_cast<const T *>(static_cast<const IndexerNode<T> *>(Base_::next()));
    }

    T *next()
    {
        return const_cast<T *>(static_cast<const T *>(static_cast<const IndexerNode<T> *>(Base_::next())));
    }
};


template<typename T, typename A = EmptyAllocator<T> > class Indexer : public GeneralTree<IndexerNode<T>, IndexerTypes, A>
{
    typedef GeneralTree<IndexerNode<T>, IndexerTypes, A> Base_;
    typedef typename Base_::PlaceBase_ PlaceBase_;


public:
    Indexer()
    {
    }

    explicit Indexer(const A &alloc) : Base_(alloc)
    {
    }


    friend void swap(Indexer<T, A> &tree1, Indexer<T, A> &tree2)
    {
        swap(static_cast<Base_ &>(tree1), static_cast<Base_ &>(tree2));
    }

    size_t check_() const
    {
#ifdef NTL_DEBUG
        size_t lev = Base_::check_();  IndexerBase::check_index_();  return lev;
#else
        return 0;
#endif
    }


    const T *at(size_t index) const
    {
        return static_cast<const T *>(static_cast<const IndexerNode<T> *>(IndexerBase::at_(index)));
    }

    T *at(size_t index)
    {
        return const_cast<T *>(static_cast<const T *>(static_cast<const IndexerNode<T> *>(IndexerBase::at_(index))));
    }

    const T *operator [] (size_t index) const
    {
        return at(index);
    }

    T *operator [] (size_t index)
    {
        return at(index);
    }

    typename Base_::ConstPlace place_at(size_t index) const
    {
        return IndexerBase::place_at_(index);
    }

    typename Base_::Place place_at(size_t index)
    {
        return IndexerBase::place_at_(index);
    }

    T *take_at(size_t index)
    {
        T *node = at(index);  static_cast<IndexerNode<T> *>(node)->remove();  return node;
    }

    void insert_at(T *node, size_t index)
    {
        insert_(node, IndexerBase::place_at_(index));
    }
};


template<typename T> class OwningIndexer : public Indexer<T, DefaultAllocator<T> >  // template typedef workaround
{
    typedef Indexer<T, DefaultAllocator<T> > Base_;

    friend void swap(OwningIndexer<T> &tree1, OwningIndexer<T> &tree2)
    {
        swap(static_cast<Base_ &>(tree1), static_cast<Base_ &>(tree2));
    }
};



template<typename T> class SimpleKey : public Comparable<SimpleKey<T> >
{
    T id_;

public:
    SimpleKey()
    {
    }

    SimpleKey(const SimpleKey<T> &key) : id_(key.id_)
    {
    }

    explicit SimpleKey(const T &key) : id_(key)
    {
    }

    void set_id(const T &key)
    {
        id_ = key;
    }

    int cmp(const SimpleKey<T> &key) const
    {
        if(id_ < key.id_)return -1;  if(id_ > key.id_)return 1;  return 0;
    }

    int cmp(const T &key) const
    {
        if(id_ < key)return -1;  if(id_ > key)return 1;  return 0;
    }

    T id() const
    {
        return id_;
    }
};



}  // end namespace NTL_



namespace NTL
{
    using NTL_::TreeNode;
    using NTL_::Tree;
    using NTL_::OwningTree;

    using NTL_::IndexerNode;
    using NTL_::Indexer;
    using NTL_::OwningIndexer;

    using NTL_::SimpleKey;
}
