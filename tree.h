// tree.h -- AA tree classes
//

#pragma once

#include "allocator.h"



namespace NTL_Internal_ {



template<typename N> class GeneralTreeNode : public N, public Heavy
{
    template<typename N1> friend class GeneralTreeBase;
    template<typename N1, typename B, typename A> friend class GeneralTree;


    typedef GeneralTreeNode<N> Node_;
    typedef typename N::Tree_ Tree_;


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
        parent_ = ptr;  type_ = tp;
        switch(tp)
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
        if(type_ == t_left)
            if(parent_->type_ == t_left_red)
            {
                parent_->parent_->rotate_right_();  return parent_;
            }
            else type_ = t_left_red;
        else if(type_ == t_right)
            if(parent_->left_ && parent_->left_->type_ == t_left_red)
            {
                parent_->left_->type_ = t_left;  return parent_;
            }
            else
            {
                parent_->rotate_left_();
                if(type_ == t_left_red)
                {
                    left_->type_ = t_left;  parent_->rotate_right_();  return this;
                }
            }
        return 0;
    }

    Node_ *remove_red_()
    {
        if(type_ == t_left_red)type_ = t_left;
        else if(type_ == t_left)
            if(parent_->right_->left_ && parent_->right_->left_->type_ == t_left_red)
            {
                parent_->right_->rotate_right_();  parent_->rotate_left_();  parent_->type_ = t_left;
            }
            else
            {
                parent_->rotate_left_();  return parent_->parent_;
            }
        else if(type_ == t_right)
            if(parent_->left_->type_ == t_left)
                if(parent_->left_->left_ && parent_->left_->left_->type_ == t_left_red)
                {
                    parent_->left_->left_->type_ = t_left;  parent_->rotate_right_();
                }
                else
                {
                    parent_->left_->type_ = t_left_red;  return parent_;
                }
            else
                if(parent_->left_->right_->left_ && parent_->left_->right_->left_->type_ == t_left_red)
                {
                    parent_->left_->rotate_left_();  parent_->rotate_right_();
                }
                else
                {
                    parent_->rotate_right_();  parent_->left_->type_ = t_left_red;
                }
        return 0;
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

    size_t check_()  // DEBUG
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


protected:
    const Node_ *prev_() const
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

    const Node_ *next_() const
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

        N::dec_index_();
        if(type_ > t_right)parent_->left_ = 0;
        else if(type_ == t_right)parent_->right_ = 0;
        else reinterpret_cast<Tree_ *>(parent_)->root_ = 0;
        parent_ = 0;
    }

    bool assigned()
    {
        return parent_ != 0;
    }
};


template<typename B> class GeneralTreeBase : public B
{
    template<typename N> friend class GeneralTreeNode;
    template<typename N, typename B1, typename A> friend class GeneralTree;


    typedef GeneralTreeNode<typename B::Node_> Node_;


    Node_ *root_;


    Node_ *root_cast_()
    {
        return reinterpret_cast<Node_ *>(this);
    }

    void swap_(GeneralTreeBase<B> &tree)
    {
        if(root_)root_->parent_ = tree.root_cast_();
        if(tree.root_)tree.root_->parent_ = root_cast_();
        NTL::swap(root_, tree.root_);
    }

public:
    GeneralTreeBase() : root_(0)
    {
    }

    size_t check_()  // DEBUG
    {
        if(!root_)return 0;  assert(root_->type_ == Node_::t_root);  return root_->check_();
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


template<typename N, typename B, typename A> class GeneralTree : private A, public GeneralTreeBase<B>, public Heavy
{
    typedef typename N::Type_ Type_;
    typedef typename N::Node_ Node_;
    typedef GeneralTreeBase<B> Tree_;
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
        N *node = create(*const_cast_(old));  if(!node)return 0;
        node->parent_ = parent;  node->left_ = node->right_ = 0;
        node->type_ = old->type_;  node->copy_index_(old);  return node;
    }

    template<typename K> PlaceBase_ find_place_(const K &key) const
    {
        PlaceBase_ res;
        for(Node_ *node = Tree_::root_; node;)
        {
            int dir = -const_cast_(node)->cmp(key);
            res.node = node;  res.dir = dir;
            if(dir > 0)node = node->right_;
            else if(dir < 0)node = node->left_;
            else return res;
        }
        return res;
    }

    void swap_(GeneralTree<N, B, A> &tree)
    {
        Tree_::swap_(tree);
    }


public:
    class Place : private PlaceBase_
    {
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
            return cast_(PlaceBase_::dir >= 0 ? PlaceBase_::node : PlaceBase_::node->prev_());
        }

        Type_ *after() const
        {
            if(!PlaceBase_::node)return 0;
            return cast_(PlaceBase_::dir <= 0 ? PlaceBase_::node : PlaceBase_::node->next_());
        }
    };

    class ConstPlace : private PlaceBase_
    {
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
            return const_cast_(PlaceBase_::dir >= 0 ? PlaceBase_::node : PlaceBase_::node->prev_());
        }

        const Type_ *after() const
        {
            if(!PlaceBase_::node)return 0;
            return const_cast_(PlaceBase_::dir <= 0 ? PlaceBase_::node : PlaceBase_::node->next_());
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
                    ptr->parent_ = 0;  remove(cast_(ptr));  Tree_::root_ = 0;  return;
                }
                ptr->parent_ = 0;  remove(cast_(ptr));
            }
        }
    }

    template<typename A1> bool copy(const GeneralTree<N, B, A1> &tree)
    {
        assert(!Tree_::root_);  if(!tree.Tree_::root_)return true;
        Node_ *node = copy_node_(Tree_::root_cast_(), tree.Tree_::root_);
        if(!node)return false;  Tree_::root_ = node;

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

    friend void swap(GeneralTree<N, B, A> &tree1, GeneralTree<N, B, A> &tree2)
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
        return const_cast<Type_ *>(const_cast<const GeneralTree<N, B, A> *>(this)->find(key));
    }

    template<typename K> const Type_ *find_prev(const K &key) const
    {
        PlaceBase_ place = find_place_(key);  if(!place.node)return 0;
        return const_cast_(place.dir >= 0 ? place.node : place.node->prev_());
    }

    template<typename K> Type_ *find_prev(const K &key)
    {
        return const_cast<Type_ *>(const_cast<const GeneralTree<N, B, A> *>(this)->find_prev(key));
    }

    template<typename K> const Type_ *find_next(const K &key) const
    {
        PlaceBase_ place = find_place_(key);  if(!place.node)return 0;
        return const_cast_(place.dir <= 0 ? place.node : place.node->next_());
    }

    template<typename K> Type_ *find_next(const K &key)
    {
        return const_cast<Type_ *>(const_cast<const GeneralTree<N, B, A> *>(this)->find_next(key));
    }

    template<typename K> Type_ *take(const K &key)
    {
        PlaceBase_ place = find_place_(key);  if(!place.dir)return 0;
        Node_ *node = const_cast<Node_ *>(place.node);
        node->remove();  return cast_(node);
    }


    void insert(N *node, const PlaceBase_ &place)
    {
        assert(place.dir);
        if(place.node)const_cast<Node_ *>(place.node)->insert_(node, place.dir > 0);
        else set_root_(node);
    }

    void insert(N *node)
    {
        insert(node, find_place_(*node));
    }

    template<typename K> Type_ *get(const K &key)
    {
        PlaceBase_ place = find_place_(key);
        if(!place.dir)return cast_(place.node);
        Type_ *node = create(key);  if(node)insert(node, place);  return node;
    }
};



class TreeNodeBase;

class TreeBase
{
    template<typename B> friend class GeneralTreeBase;

    typedef TreeNodeBase Node_;
};

class TreeNodeBase
{
    template<typename N> friend class GeneralTreeNode;
    template<typename N, typename B, typename A> friend class GeneralTree;

    typedef GeneralTreeBase<TreeBase> Tree_;


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


template<typename T> class TreeNode : public GeneralTreeNode<TreeNodeBase>
{
    template<typename N, typename B, typename A> friend class GeneralTree;


    typedef T Type_;
    typedef GeneralTreeNode<TreeNodeBase> Node_;


public:
    const T *prev() const
    {
        return static_cast<const T *>(static_cast<const TreeNode<T> *>(prev_()));
    }

    T *prev()
    {
        return const_cast<T *>(static_cast<const T *>(static_cast<const TreeNode<T> *>(prev_())));
    }

    const T *next() const
    {
        return static_cast<const T *>(static_cast<const TreeNode<T> *>(next_()));
    }

    T *next()
    {
        return const_cast<T *>(static_cast<const T *>(static_cast<const TreeNode<T> *>(next_())));
    }
};


template<typename T, typename A = EmptyAllocator<T> > class Tree : public GeneralTree<TreeNode<T>, TreeBase, A>
{
    typedef GeneralTree<TreeNode<T>, TreeBase, A> Self_;

    friend void swap(Tree<T, A> &tree1, Tree<T, A> &tree2)
    {
        swap(static_cast<Self_ &>(tree1), static_cast<Self_ &>(tree2));
    }
};


template<typename T> class OwningTree : public Tree<T, DefaultAllocator<T> >  // template typedef workaround
{
    typedef Tree<T, DefaultAllocator<T> > Self_;

    friend void swap(OwningTree<T> &tree1, OwningTree<T> &tree2)
    {
        swap(static_cast<Self_ &>(tree1), static_cast<Self_ &>(tree2));
    }
};



template<typename T> class SimpleKey : public Comparable<SimpleKey<T>, T>, public Comparable<SimpleKey<T> >
{
    T id_;

public:
    SimpleKey()
    {
    }

    SimpleKey(const SimpleKey<T> &key) : id_(key.id_)
    {
    }

    SimpleKey(const T &key) : id_(key)
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



}  // end namespace NTL_Internal_



namespace NTL
{
    using NTL_Internal_::TreeNode;
    using NTL_Internal_::Tree;
    using NTL_Internal_::OwningTree;

    using NTL_Internal_::SimpleKey;
}
