// tree.h -- AA tree classes
//

#pragma once

#include "allocator.h"



template<typename T> class TreeNode : public Heavy
{
    template<typename T1, typename A> friend class Tree;


    enum NodeType_
    {
        t_root = 0, t_right, t_left, t_left_red
    };


    TreeNode<T> *parent_, *left_, *right_;
    NodeType_ type_;


    const TreeNode<T> *first_() const
    {
        for(const TreeNode<T> *node = this;; node = node->left_)if(!node->left_)return node;
    }

    const TreeNode<T> *last_() const
    {
        for(const TreeNode<T> *node = this;; node = node->right_)if(!node->right_)return node;
    }


    void set_parent_(TreeNode<T> *ptr, NodeType_ tp)
    {
        parent_ = ptr;  type_ = tp;
        switch(tp)
        {
        case t_root:  *reinterpret_cast<TreeNode<T> **>(ptr) = this;  return;
        case t_right:  ptr->right_ = this;  return;
        default:  ptr->left_ = this;  return;
        }
    }

    void rotate_left_()
    {
        TreeNode<T> *next = right_;  next->set_parent_(parent_, type_);
        if(right_ = next->left_)
        {
            right_->parent_ = this;  right_->type_ = t_right;
        }
        next->left_ = this;  parent_ = next;  type_ = t_left_red;
    }

    void rotate_right_()
    {
        TreeNode<T> *next = left_;  next->set_parent_(parent_, type_);
        if(left_ = next->right_)
        {
            left_->parent_ = this;  left_->type_ = t_left;
        }
        next->right_ = this;  parent_ = next;  type_ = t_right;
    }

    TreeNode<T> *make_red_()
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

    TreeNode<T> *remove_red_()
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

    void swap_(TreeNode<T> *node)
    {
        TreeNode<T> *l = left_, *r = right_, *p = parent_;  NodeType_ t = type_;
        if(node->parent_ == this)
        {
            parent_ = node;  type_ = node->type_;  node->set_parent_(p, t);
            if(type_ == t_right)
            {
                if(left_ = node->left_)left_->parent_ = this;  if(node->left_ = l)l->parent_ = node;
                if(right_ = node->right_)right_->parent_ = this;  node->right_ = this;
            }
            else
            {
                if(left_ = node->left_)left_->parent_ = this;  node->left_ = this;
                if(right_ = node->right_)right_->parent_ = this;  if(node->right_ = r)r->parent_ = node;
            }
        }
        else
        {
            set_parent_(node->parent_, node->type_);  node->set_parent_(p, t);
            if(left_ = node->left_)left_->parent_ = this;  if(node->left_ = l)l->parent_ = node;
            if(right_ = node->right_)right_->parent_ = this;  if(node->right_ = r)r->parent_ = node;
        }
    }

    void insert_(TreeNode<T> *node, bool after)
    {
        assert(parent_ && !node->parent_);  node->parent_ = this;  node->left_ = node->right_ = 0;
        if(after)
        {
            assert(right_ == 0);  right_ = node;  node->type_ = t_right;
        }
        else
        {
            assert(left_ == 0);  left_ = node;  node->type_ = t_left;
        }
        do node = node->make_red_();
        while(node);
    }

    void check_()  // DEBUG
    {
        assert(type_ != t_left_red || parent_->type_ != t_left_red);
        if(left_)
        {
            assert(left_->parent_ == this && left_->type_ > t_right);
            assert(right_ || left_->type_ != t_left && !left_->left_ && !left_->right_);
            left_->check_();
        }
        if(right_)
        {
            assert(right_->parent_ == this && right_->type_ == t_right && left_);
            right_->check_();
        }
        assert(type_ != t_root || *reinterpret_cast<TreeNode<T> **>(parent_) == this);
    }


    struct Place_
    {
        const TreeNode<T> *node;
        int dir;

        Place_() : node(0), dir(-1)
        {
        }

        void set(const TreeNode<T> *ptr, int d)
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
    TreeNode() : parent_(0)
    {
    }

    ~TreeNode()
    {
        if(parent_)remove();
    }


    const T *prev() const
    {
        assert(parent_ != 0);  const TreeNode<T> *node = this;
        if(!left_)
        {
            while(node->type_ > t_right)node = node->parent_;
            return node->type_ ? static_cast<const T *>(node->parent_) : 0;
        }
        for(node = node->left_;; node = node->right_)
            if(!node->right_)return static_cast<const T *>(node);
    }

    T *prev()
    {
        return const_cast<T *>(const_cast<const TreeNode<T> *>(this)->prev());
    }

    const T *next() const
    {
        assert(parent_ != 0);  const TreeNode<T> *node = this;
        if(!right_)
        {
            while(node->type_ == t_right)node = node->parent_;
            return node->type_ ? static_cast<const T *>(node->parent_) : 0;
        }
        for(node = node->right_;; node = node->left_)
            if(!node->left_)return static_cast<const T *>(node);
    }

    T *next()
    {
        return const_cast<T *>(const_cast<const TreeNode<T> *>(this)->next());
    }


    void remove()
    {
        assert(parent_ != 0);
        while(left_)swap_(const_cast<TreeNode<T> *>(left_->last_()));

        TreeNode<T> *node = this;
        do node = node->remove_red_();
        while(node);

        if(type_ > t_right)parent_->left_ = 0;
        else if(type_ == t_right)parent_->right_ = 0;
        else *reinterpret_cast<TreeNode<T> **>(parent_) = 0;
        parent_ = 0;
    }

    bool assigned()
    {
        return parent_ != 0;
    }
};


template<typename T, typename A = EmptyAllocator<T> > class Tree : public Heavy
{
    A alloc_;
    TreeNode<T> *root_;


    typedef typename TreeNode<T>::Place_ PlaceBase_;


    void set_root_(TreeNode<T> *node)
    {
        assert(root_ == 0);
        root_ = node;  node->parent_ = reinterpret_cast<TreeNode<T> *>(&root_);
        node->left_ = node->right_ = 0;  node->type_ = TreeNode<T>::t_root;
    }

    TreeNode<T> *copy_node_(TreeNode<T> *parent, TreeNode<T> *old)
    {
        TreeNode<T> *node = alloc_.create(*static_cast<T *>(old));
        node->parent_ = parent;  node->left_ = node->right_ = 0;
        node->type_ = old->type_;  return node;
    }

    template<typename K> PlaceBase_ find_place_(const K &key) const
    {
        PlaceBase_ res;
        for(TreeNode<T> *node = root_; node;)
        {
            int dir = -static_cast<T *>(node)->cmp(key);  res.set(node, dir);
            if(dir > 0)node = node->right_;
            else if(dir < 0)node = node->left_;
            else return res;
        }
        return res;
    }

    void swap_(Tree<T, A> &tree)
    {
        if(root_)root_->parent_ = reinterpret_cast<TreeNode<T> *>(&tree.root_);
        if(tree.root_)tree.root_->parent_ = reinterpret_cast<TreeNode<T> *>(&root_);
        ::swap(root_, tree.root_);
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

        Place(T *node, bool after) : PlaceBase_(node, after)
        {
        }

        Place &operator = (const Place &place)
        {
            *static_cast<PlaceBase_ *>(this) = place;  return *this;
        }

        T *node() const
        {
            return const_cast<T *>(static_cast<const T *>(PlaceBase_::node()));
        }

        int direction() const
        {
            return PlaceBase_::dir;
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

        ConstPlace(const T *node, bool after) : PlaceBase_(node, after)
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

        const T *node() const
        {
            return static_cast<const T *>(PlaceBase_::node);
        }

        int direction() const
        {
            return PlaceBase_::dir;
        }
    };


    void check_()  // DEBUG
    {
        if(!root_)return;  assert(root_->type_ == TreeNode<T>::t_root);  root_->check_();
    }


    Tree() : root_(0)
    {
    }

    explicit Tree(const A &alloc) : alloc_(alloc), root_(0)
    {
    }

    ~Tree()
    {
        clear();
    }


    const T *first() const
    {
        return root_ ? static_cast<const T *>(root_->first_()) : 0;
    }

    T *first()
    {
        return root_ ? const_cast<T *>(static_cast<const T *>(root_->first_())) : 0;
    }

    const T *last() const
    {
        return root_ ? static_cast<const T *>(root_->last_()) : 0;
    }

    T *last()
    {
        return root_ ? const_cast<T *>(static_cast<const T *>(root_->last_())) : 0;
    }


    void clear()
    {
        if(root_)for(TreeNode<T> *node = root_;;)
        {
            if(node->left_)node = node->left_;
            else if(node->right_)node = node->right_;
            else
            {
                TreeNode<T> *ptr = node;  node = node->parent_;
                if(ptr->type_ > TreeNode<T>::t_right)node->left_ = 0;
                else if(ptr->type_ == TreeNode<T>::t_right)node->right_ = 0;
                else
                {
                    ptr->parent_ = 0;  alloc_.remove(static_cast<T *>(ptr));  root_ = 0;  return;
                }
                ptr->parent_ = 0;  alloc_.remove(static_cast<T *>(ptr));
            }
        }
    }

    template<typename A1> bool copy(const Tree<T, A1> &tree)
    {
        assert(root_ == 0);  if(!tree.root_)return true;

        TreeNode<T> *node = root_ =
            copy_node_(reinterpret_cast<TreeNode<T> *>(&root_), tree.root_);
        for(TreeNode<T> *old = tree.root_;;)
            if(!node->left_ && old->left_)
            {
                if(!(node = node->left_ = copy_node_(node, old = old->left_)))return false;
            }
            else if(!node->right_ && old->right_)
            {
                if(!(node = node->right_ = copy_node_(node, old = old->right_)))return false;
            }
            else
            {
                if(old->type_ == TreeNode<T>::t_root)return true;
                old = old->parent_;  node = node->parent_;
            }
    }


    template<typename K> ConstPlace find_place(const K &key) const
    {
        return find_place_(key);
    }

    template<typename K> Place find_place(const K &key)
    {
        return find_place_(key);
    }

    template<typename K> const T *find(const K &key) const
    {
        PlaceBase_ place = find_place_(key);  return place.dir ? 0 : static_cast<const T *>(place.node);
    }

    template<typename K> T *find(const K &key)
    {
        return const_cast<T *>(const_cast<const Tree<T, A> *>(this)->find(key));
    }

    template<typename K> const T *find_prev(const K &key) const
    {
        PlaceBase_ place = find_place_(key);  if(!place.node)return 0;
        return static_cast<const T *>(place.dir >= 0 ? place.node : place.node->prev());
    }

    template<typename K> T *find_prev(const K &key)
    {
        return const_cast<T *>(const_cast<const Tree<T, A> *>(this)->find_prev(key));
    }

    template<typename K> const T *find_next(const K &key) const
    {
        PlaceBase_ place = find_place_(key);  if(!place.node)return 0;
        return static_cast<const T *>(place.dir <= 0 ? place.node : place.node->next());
    }

    template<typename K> T *find_next(const K &key)
    {
        return const_cast<T *>(const_cast<const Tree<T, A> *>(this)->find_next(key));
    }

    void insert(T *node, const PlaceBase_ &place)
    {
        assert(place.dir != 0);
        if(place.node)const_cast<TreeNode<T> *>(place.node)->insert_(node, place.dir > 0);
        else set_root_(node);
    }

    void insert(T *node)
    {
        insert(node, find_place_(*node));
    }

    template<typename K> T *get(const K &key)
    {
        PlaceBase_ place = find_place_(key);
        if(!place.dir)return const_cast<T *>(static_cast<const T *>(place.node));
        T *node = alloc_.create(key);  if(node)insert(node, place);  return node;
    }

    friend void swap(Tree<T, A> &tree1, Tree<T, A> &tree2)
    {
        tree1.swap_(tree2);
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

template<typename T> class OwningTree : public Tree<T, DefaultAllocator<T> >  // template typedef workaround
{
    friend void swap(OwningTree<T> &tree1, OwningTree<T> &tree2)
    {
        swap(static_cast<Tree<T, DefaultAllocator<T> > &>(tree1), static_cast<Tree<T, DefaultAllocator<T> > &>(tree2));
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
