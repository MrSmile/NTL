#include <iostream>
#include "literal.h"
#include "pointer.h"
#include "tree.h"
#include "list.h"

using namespace std;
using namespace NTL;



void test_string()
{
    cout << "\n=== Testing Strings ===\n";
    {
        String copy, str = "~~~";
        str = "Zzz: " + str + '\n';
        copy = "...";  copy = str;
        str += Literal("Next line") + '\n';
        str += "0 " + Literal("1 ") + "2 " + "3 " + "3 " + "5 " + "6 " + "7 " + "8 " + "9 " + "10 " + '\n';
        str += '\n';  cout << copy.data() << str.data();

        if(str > "ZZZ")cout << "Greater than 'ZZZ'\n";
    }
    cout << endl;
}


struct Node : public SimpleKey<int>
{
    static int count;

    Node(int n = 777) : SimpleKey<int>(n)
    {
        cout << "Created " << id() << ", total " << ++count << endl;
    }

    Node(const Node &node) : SimpleKey<int>(node.id() + 1000)
    {
        cout << "Copied " << node.id() << " -> " << id() << ", total " << ++count << endl;
    }

    ~Node()
    {
        cout << "Deleted " << id() << ", left " << --count << endl;
    }

    void *operator new(size_t size, const std::nothrow_t &) throw()
    {
        const int max = 100;
        if(count >= max)return 0;  return new_nt char[size];
    }
};

int Node::count = 0;

void test_pointer()
{
    cout << "\n=== Testing Pointers ===\n";
    {
        Pointer<Node> del(new_nt Node(555));
        Pointer<Node> node(new_nt Node(666));
        Array<Node> array(10);  del = new_nt Node(888);
        delete node.detach();
    }
    cout << endl;
}


struct NodeT1 : public TreeNode<NodeT1>, public Node
{
    NodeT1(int n) : Node(n)
    {
    }

    NodeT1(const Node &node) : Node(node)
    {
    }
};

struct NodeT2 : public IndexerNode<NodeT2>, public Node
{
    NodeT2(int n) : Node(n)
    {
    }

    NodeT2(const Node &node) : Node(node)
    {
    }
};

void test_tree()
{
    cout << "\n=== Testing Trees, Stage 1 ===\n";
    {
        OwningTree<NodeT1> tree;
        for(int i = 0; i < 10; i++)tree.get(i);
        tree.check_();
        {
            OwningTree<NodeT1> copy;
            if(!copy.copy(tree))cout << "Copy failed!\n";
            else delete copy.find(1005)->next();
            copy.check_();  swap(tree, copy);  copy.check_();
        }
        tree.check_();
        for(NodeT1 *node = tree.first(); node; node = node->next())
            cout << node->id() << ' ';  cout << endl;
        for(NodeT1 *node = tree.last(); node; node = node->prev())
            cout << node->id() << ' ';  cout << endl;
    }
    cout << "\n=== Testing Trees, Stage 2 ===\n";
    {
        OwningIndexer<NodeT2> tree;
        for(int i = 0; i < 10; i++)tree.get(i);
        {
            OwningIndexer<NodeT2> copy;  copy.check_();
            if(!copy.copy(tree))cout << "Copy failed!\n";
            else delete copy.find(1005)->next();
            copy.check_();  swap(tree, copy);  copy.check_();
        }
        cout << "Index of 1006: " << tree.find_place(1006).index() << endl;
        tree.insert(new_nt NodeT2(444), 4);  tree.check_();  size_t index = 0;
        for(NodeT2 *node = tree.first(); node; node = node->next(), index++)
            assert(tree[index] == node && node->index() == index);
        for(NodeT2 *node = tree.first(); node; node = node->next())
            cout << node->id() << ' ';  cout << endl;
        for(NodeT2 *node = tree.last(); node; node = node->prev())
            cout << node->id() << ' ';  cout << endl;
    }
    cout << endl;
}


struct NodeL1 : public StackNode<NodeL1>, public Node
{
    NodeL1(int n) : Node(n)
    {
    }

    NodeL1(const Node &node) : Node(node)
    {
    }
};

struct NodeL2 : public SimpleListNode<NodeL2>, public Node
{
    NodeL2(int n) : Node(n)
    {
    }

    NodeL2(const Node &node) : Node(node)
    {
    }
};

struct NodeL3 : public ListNode<NodeL3>, public Node
{
    NodeL3(int n) : Node(n)
    {
    }

    NodeL3(const Node &node) : Node(node)
    {
    }
};

void test_list()
{
    cout << "\n=== Testing Lists, Stage 1 ===\n";
    {
        OwningStack<NodeL1> list;
        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL1(i));
        list.first()->next()->next()->insert_next(new_nt NodeL1(777));
        delete list.first()->next()->take_next();
        OwningStack<NodeL1> move, copy;
        if(!copy.copy(list))cout << "Copy failed!\n";
        for(;;)
        {
            NodeL1 *node = list.take_first();  if(!node)break;
            cout << node->id() << ' ';  move.prepend(node);
        }
        cout << endl;  swap(list, move);
        for(NodeL1 *node = list.first(); node; node = node->next())
            cout << node->id() << ' ';  cout << endl;
    }
    cout << "\n=== Testing Lists, Stage 2 ===\n";
    {
        OwningSimpleList<NodeL2> list;
        for(int i = 0; i < 10; i++)list.append(new_nt NodeL2(i));
        list.insert(new_nt NodeL2(777), list.first()->next()->next());
        delete list.take(list.first());  list.prepend(new_nt NodeL2(666));
        OwningSimpleList<NodeL2> move, copy;
        if(!copy.copy(list))cout << "Copy failed!\n";
        for(;;)
        {
            NodeL2 *node = list.take_first();  if(!node)break;
            cout << node->id() << ' ';  move.prepend(node);
        }
        cout << endl;  swap(list, move);
        if(!list.prepend_copy(copy))cout << "Merge copy failed!\n";
        list.append(copy);
        for(NodeL2 *node = list.first(); node; node = node->next())
            cout << node->id() << ' ';  cout << endl;
    }
    cout << "\n=== Testing Lists, Stage 3 ===\n";
    {
        OwningList<NodeL3> list;
        for(int i = 0; i < 10; i++)list.prepend(new_nt NodeL3(i));
        list.first()->next()->next()->insert_next(new_nt NodeL3(777));
        list.first()->next()->next()->insert_prev(new_nt NodeL3(666));
        delete list.first()->next();
        OwningList<NodeL3> move, copy;
        if(!copy.copy(list))cout << "Copy failed!\n";
        for(;;)
        {
            NodeL3 *node = list.take_first();  if(!node)break;
            cout << node->id() << ' ';  move.prepend(node);
        }
        cout << endl;  swap(list, move);
        for(NodeL3 *node = list.first(); node; node = node->next())
            cout << node->id() << ' ';  cout << endl;
    }
    cout << endl;
}


void time_tree();

int main(int n, char **arg)
{
    for(int i = 1; i < n; i++)
        if(arg[i] == Literal("string"))test_string();
        else if(arg[i] == Literal("pointer"))test_pointer();
        else if(arg[i] == Literal("tree"))
        {
            test_tree();  time_tree();
        }
        else if(arg[i] == Literal("list"))test_list();

    if(n <= 1)cout << "Usage: test [string] [pointer] [tree] [list]\n";
    return 0;
}
